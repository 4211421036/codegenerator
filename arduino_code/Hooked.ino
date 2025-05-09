/*
    Copyright (c) 2018 Gurpreet Bal https://github.com/ardyesp/ESPWebDAV
    Copyright (c) 2020 David Gauchard https://github.com/d-a-v/ESPWebDAV
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
      derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
    SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
    OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
    OF SUCH DAMAGE.

*/

/*  Using the WebDAV server

    From windows file explorer,
        \\ESPWebDAV.local\
        or http://ESPWebDAV.local/
        or Map Network Drive -> Connect to http://ESPWebDAV.local/
           subst w: \\ESPWebDAV.local/DavWWWRoot
    From macOS Finder > command-K > http://ESPWebDAV.local/
        (do not select anonymous to have write access)
    From macOS cmdline:
        mkdir -p /tmp/esp; mount_webdav -S -i -v esp8266 http://ESPWebDAV.local/ /tmp/esp && echo OK
    From linux:
        use mount -t davs2 http://ESPWebDAV.local/ /mnt/
        use gio/gvfs/nautilus/YourFileExplorer http://ESPWebDAV.local/

    When running emulation on host (script ./run), use one of these URLs instead:
        http://ESPWebDAV.local:9080/
        http://127.0.0.1:9080/
        http://local-ip-address:9080/
        subst w: \\ESPWebDAV.local@9080/DavWWWRoot
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include <WebDav4WebServer.h>

#if !WEBSERVER_HAS_HOOK
#error This sketch needs ESP8266WebServer::HookFunction and ESP8266WebServer::addHook
#endif

#define HOSTNAME    "ESPWebDAV"

#ifndef STASSID
#define STASSID "ssid"
#define STAPSK "psk"
#endif

#define DAVROOT "/"         // this is the WebDAV root-URL directory, / is allowed

//FS& gfs = SPIFFS;
FS& gfs = LittleFS;
//FS& gfs = SDFS;

ESP8266WebServer server(80);

ESPWebDAVCore dav;

static const char TEXT_PLAIN[] PROGMEM = "text/plain";

void notFound ()
{
    String nf = DAVROOT;
    nf += ESP8266WebServer::urlDecode(server.uri());
    Serial.printf("User request for HTTP file '%s' from '" DAVROOT "'\n", nf.c_str() + sizeof(DAVROOT));
    // open file 'DAVROOT nf' (/dav/userfilename)
    File f = gfs.open(nf.c_str(), "r");
    if (!f)
    {
        Serial.printf("not found: '%s'\n", nf.c_str());
        server.send(404, FPSTR(TEXT_PLAIN), "webserver's notfound/404");
    }
    else
    {
        // This is a reachable piece of code if the file is
        // explicitally ignored in dav.setIgnored({}),
        // or if DAVROOT is not '/'.
        Serial.printf("found, streaming with HTTP (not webdav)\n");
        server.streamFile(f, F("application/octet-stream"));
    }
}

// ------------------------
void setup()
{
    // ------------------------
    WiFi.persistent(false);
    WiFi.hostname(HOSTNAME);
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    WiFi.begin(STASSID, STAPSK);
    Serial.println("Connecting to " STASSID " ...");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to "); Serial.println(STASSID);
    Serial.print("IP address: "); Serial.println(WiFi.localIP());
    Serial.print("RSSI: "); Serial.println(WiFi.RSSI());
    Serial.print("Mode: "); Serial.println(WiFi.getPhyMode());

    MDNS.begin(HOSTNAME);

    gfs.begin();
    gfs.mkdir(DAVROOT);
    dav.begin(&gfs);
    dav.setTransferStatusCallback([](const char* name, int percent, bool receive)
    {
        Serial.printf("%s: '%s': %d%%\n", receive ? "recv" : "send", name, percent);
    });

    // setup webserver's Hook for WebDAV,
    // optionally: DAVROOT/some/path on URL is translated to 'somewhere/else/some/path' on FS
    server.addHook(hookWebDAVForWebserver(DAVROOT, dav, /*optional, replace DAVROOT by this on FS:*/ "/"));

    ////
    // Allow some paths within the WebDAV namespace to be served by the regular webwerver
    //
    // 1. provide a callback to verify what to ignore to WebDAV (=> thus served by webserver)
    dav.setIgnored([] (const String& uri) { return uri == F("/index.html") || uri == F("/another.html") || uri == F("/notfound.html"); });
    //
    // 2. setup regular web pages callbacks (only meaningful when paths are ignored by WebDAV, like above)
    server.on(F("/index.html"), [] () { server.send_P(200, PSTR("text/html"), PSTR("<meta http-equiv=\"Refresh\" content=\"0;url=another.html\">")); });
    server.on(F("/another.html"), [] () { server.send_P(200, PSTR("text/html"), PSTR("<button style=\"background-color:red;color:orange;\" onclick=\"window.location.href='notfound.html';\">heaven</button>")); });
    //
    ////

    server.onNotFound(notFound); // useless when DAVROOT is "/" because WebDAV handles locations first (unless ignored)
    server.begin();
    Serial.println("HTTP server started");
    Serial.println("WebDAV server started");
}

void help()
{
    Serial.printf("interactive: F/ormat D/ir C/reateFile\n");

    uint32_t freeHeap;
    uint32_t maxBlock;
    uint8_t fragmentation;
    ESP.getHeapStats(&freeHeap, &maxBlock, &fragmentation);
    Serial.printf("Heap stats: free heap: %u - max block: %u - fragmentation: %u%%\n",
                  freeHeap, maxBlock, fragmentation);
}

// ------------------------
void loop()
{
    MDNS.update();
    server.handleClient();

    int c = Serial.read();
    if (c > 0)
    {
        if (c == 'F')
        {
            Serial.println("formatting...");
            if (gfs.format())
                Serial.println("Success");
            else
                Serial.println("Failure");
        }
        else if (c == 'D')
        {
            Serial.printf(">>>>>>>> dir /\n");
            dav.dir("/", &Serial);
            Serial.printf("<<<<<<<< dir\n");
        }
        else if (c == 'C')
        {
            auto f = gfs.open("readme.md", "w");
            f.printf("hello\n");
            f.close();
        }
        else
            help();
    }

    // ------------------------
}
