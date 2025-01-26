/****************************************************************************************************************************
  AsyncCaptivePortal.ino

  AsyncDNSServer_RP2040W is an Async DNS_Server library for the RP2040W with CYW43439 WiFi
  
  Based on and modified from ESPAsyncDNSServer Library (https://github.com/devyte/ESPAsyncDNSServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncDNSServer_RP2040W
 *****************************************************************************************************************************/

#include "defines.h"

#include <AsyncDNSServer_RP2040W.h>
#include <AsyncWebServer_RP2040W.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 100, 1);

AsyncDNSServer dnsServer;
AsyncWebServer server(80);

int status = WL_IDLE_STATUS;

String responseHTML = ""
                      "<!DOCTYPE html><html lang='en'><head>"
                      "<meta name='viewport' content='width=device-width'>"
                      "<title>RP2040W-CaptivePortal</title></head><body>"
                      "<h1>Hello World from RP2040W!</h1><p>This is a captive portal example."
                      " All requests will be redirected here.</p></body></html>";

void handleNotFound(AsyncWebServerRequest *request)
{
  request->send(200, "text/html", responseHTML);
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("Local IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup() 
{
  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  Serial.print("\nStart AsyncCaptivePortal on ");  Serial.println(BOARD_NAME);
  Serial.println(ASYNC_DNS_SERVER_RP2040W_VERSION);

#if defined(ASYNC_DNS_SERVER_RP2040W_VERSION_MIN)
  if (ASYNC_DNS_SERVER_RP2040W_VERSION_INT < ASYNC_DNS_SERVER_RP2040W_VERSION_MIN)
  {
    Serial.print("Warning. Must use this example on Version equal or later than : ");
    Serial.println(ASYNC_DNS_SERVER_RP2040W_VERSION_MIN_TARGET);
  }
#endif  

  ///////////////////////////////////
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  Serial.print(F("Connecting to SSID: "));
  Serial.println(ssid);

  status = WiFi.begin(ssid, pass);

  delay(1000);
   
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED)
  {
    delay(500);
        
    // Connect to WPA/WPA2 network
    status = WiFi.status();
  }

  printWifiStatus();

  ///////////////////////////////////

  // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
  dnsServer.setTTL(300);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is AsyncDNSReplyCode::NonExistentDomain
  dnsServer.setErrorReplyCode(AsyncDNSReplyCode::ServerFailure);

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  server.onNotFound(handleNotFound);
  
  server.begin();
  
  Serial.print(F("HTTP DNSServer is @ IP : "));
  Serial.println(apIP);
}

void loop() 
{
}
