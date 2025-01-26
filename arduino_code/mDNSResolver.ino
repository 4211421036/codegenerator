/* Copyright (C) 2021 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's mDNSResolver.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Arduino.h>

// This block will leverage a compiler function that allows checking
// a header file.  If the compiler doesn't support that function, it
// should skip the rest.
//
// If the compiler does support the check, it will look for "secret.h"
// in the src/ directory to add in your local AP SSID anf password. If
// it doesn't exist, or if the compiler you are using does not support
// this function, it should skip it.
//
// If you do not or can not use this functionality, be sure to edit
// the WIFI_AP and WIFI_PW macros below.

#if defined(__has_include)
#if __has_include("secret.h")
#include "secret.h"
#else
#warning "No 'secret.h' header found, be sure to edit the WIFI_PW and WIFI_AP macros below."
#endif
#else
#warning "Compiler does not support checking for 'secret.h', be sure to edit the WIFI_PW and WIFI_AP macros below."
#endif

// Not required for lib, but this is used in the example for easy
// logging.  It may be found at:
// https://github.com/thijse/Arduino-Log
#include <ArduinoLog.h>

// Include the resolver lib
#include "mDNSResolver.h"

#ifndef WIFI_AP
#define WIFI_AP "SSID"
#define WIFI_PW "Password"
#define NAME_TO_RESOLVE "somedevice.local" // Edit this line to a local device name for testing
#endif

const char *wifi_ap = WIFI_AP;
const char *wifi_pw = WIFI_PW;
const char *resolveName = NAME_TO_RESOLVE;

using namespace mDNSResolver;
WiFiClient wifiClient;
WiFiUDP udp;
Resolver resolver(udp);

void printTimestamp(Print *_logOutput)
{

    // Division constants
    const unsigned long MSECS_PER_SEC = 1000;
    const unsigned long SECS_PER_MIN = 60;
    const unsigned long SECS_PER_HOUR = 3600;
    const unsigned long SECS_PER_DAY = 86400;

    // Total time
    const unsigned long msecs = millis();
    const unsigned long secs = msecs / MSECS_PER_SEC;

    // Time in components
    const unsigned long MilliSeconds = msecs % MSECS_PER_SEC;
    const unsigned long Seconds = secs % SECS_PER_MIN;
    const unsigned long Minutes = (secs / SECS_PER_MIN) % SECS_PER_MIN;
    const unsigned long Hours = (secs % SECS_PER_DAY) / SECS_PER_HOUR;

    // Time as string
    char timestamp[20];
    sprintf(timestamp, "%02d:%02d:%02d.%03d ", (int)Hours, (int)Minutes, (int)Seconds, (int)MilliSeconds);
    _logOutput->print(timestamp);
}

void printLogLevel(Print *_logOutput, int logLevel)
{
    /// Show log description based on log level
    switch (logLevel)
    {
    default:
    case 0:
        _logOutput->print("SILENT\t");
        break;
    case 1:
        _logOutput->print("FATAL\t");
        break;
    case 2:
        _logOutput->print("ERROR\t");
        break;
    case 3:
        _logOutput->print("WARNING\t");
        break;
    case 4:
        _logOutput->print("INFO\t");
        break;
    case 5:
        _logOutput->print("TRACE\t");
        break;
    case 6:
        _logOutput->print("VERBOSE\t");
        break;
    }
}

void printPrefix(Print *_logOutput, int logLevel)
{
    printTimestamp(_logOutput);
    printLogLevel(_logOutput, logLevel);
}

void printSuffix(Print *_logOutput, int logLevel)
{
    _logOutput->print("");
}

void setSerial()
{
    Serial.begin(BAUD);
    Serial.flush();
    Serial.println();
    Log.begin(LOG_LEVEL_VERBOSE, &Serial, false);
    Log.setPrefix(printPrefix); // Set log prefix similar to NLog
    Log.setSuffix(printSuffix); // Set log suffix
    Log.notice(F("Serial logging started at %l." CR), BAUD);
}

void setWiFi()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ap, wifi_pw);
    Log.notice(F("Waiting for connection to access point.."));
    int counterWiFi = 0;
    while (WiFi.status() != WL_CONNECTED && counterWiFi < 20)
    {
        Serial.print(F("."));
        delay(500);
        counterWiFi++;
    }
    Serial.println();
    if (WiFi.status() != WL_CONNECTED)
    {
        Log.error(F("Unable to connect to access point." CR));
        while (true) yield();    // Sit here and do nothing
    }
    Log.notice(F("Connected to AP: %s, IP: %s, GW: %s, SN: %s, DNS: %s." CR),
               WiFi.SSID().c_str(),
               WiFi.localIP().toString().c_str(),
               WiFi.gatewayIP().toString().c_str(),
               WiFi.subnetMask().toString().c_str(),
               WiFi.dnsIP().toString().c_str());
    delay(200);

    if (!MDNS.begin(WiFi.hostname()))
    {
        Log.error(F("Failed to begin mDNS." CR));
        while (true) yield();    // Sit here and do nothing
    }
    else
    {
        MDNS.addService("tcp", "tcp", 80);
        MDNS.addService("http", "tcp", 80);
        Log.notice(F("MDNS started, responding to %s on %s." CR),
                   WiFi.hostname().c_str(),
                   WiFi.localIP().toString().c_str());
    }
}

void setup()
{
    setSerial();
    setWiFi();

    Log.notice(F("Test script for %s, %s branch, version %s [%s]." CR),
        resolver.project(),
        resolver.branch(),
        resolver.version(),
        resolver.build()
    );

    Log.notice(F("Resolving %s." CR), resolveName);
    resolver.setLocalIP(WiFi.localIP());

    IPAddress ip = resolver.search(resolveName);
    if (ip != INADDR_NONE)
    {
        Log.notice(F("Resolved %s to IP address: %p" CR), resolveName, ip);
    }
    else
    {
        Log.error(F("Unable to resolve %s." CR), resolveName);
    }
}

void loop()
{
    // Required to clear the UDP buffer, as we can't unjoin a multicast group yet
    resolver.loop();
    MDNS.update();
}
