/****************************************************************************************************************************
  AsyncDNSServerFull.ino

  AsyncDNSServer_RP2040W is an Async DNS_Server library for the RP2040W with CYW43439 WiFi
  
  Based on and modified from ESPAsyncDNSServer Library (https://github.com/devyte/ESPAsyncDNSServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncDNSServer_RP2040W
 *****************************************************************************************************************************/

#include "defines.h"

#include <AsyncDNSServer_RP2040W.h>
#include <AsyncWebServer_RP2040W.h>

const byte DNS_PORT = 53;

IPAddress apIP;

AsyncDNSServer dnsServer;

AsyncWebServer server(80);

int status = WL_IDLE_STATUS;

void handleNotFound(AsyncWebServerRequest *request)
{
  String message = "Hello World from " + String(BOARD_NAME) + " using CYW43439 WiFi\n\n";
  message += "URI: ";
  message += request->url();

  request->send(200, "text/plain", message);
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

  Serial.print("\nStart AsyncDNSServerFull on "); Serial.println(BOARD_NAME);
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

  // start DNS server for a specific domain name
  dnsServer.start(DNS_PORT, "*", apIP);

  // simple HTTP server to see that DNS server is working
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) 
  {
    request->send(200, "text/plain", "Hello from DNSServer running on " + String(BOARD_NAME));
  });

  server.onNotFound(handleNotFound);
  
  server.begin();
  
  Serial.print(F("HTTP DNSServer is @ IP : "));
  Serial.println(apIP);
}

void loop() 
{
}
