/****************************************************************************************************************************
  AsyncCaptivePortalAdvanced.ino

  AsyncDNSServer_RP2040W is an Async DNS_Server library for the RP2040W with CYW43439 WiFi
  
  Based on and modified from ESPAsyncDNSServer Library (https://github.com/devyte/ESPAsyncDNSServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncDNSServer_RP2040W
 *****************************************************************************************************************************/

#include "defines.h"

#include <AsyncDNSServer_RP2040W.h>
#include <AsyncWebServer_RP2040W.h>

/*
   This example serves a "hello world".

   Now the RP2040W is in your network. You can reach it through http://192.168.x.x/

   This is a captive portal because it will redirect any http request to http://192.168.x.x/
*/

// DNS server
const byte DNS_PORT = 53;

AsyncDNSServer dnsServer;

// Web server
AsyncWebServer server(80);

IPAddress apIP;

int status = WL_IDLE_STATUS;

/** Is this an IP? */
bool isIp(String str)
{
  for (size_t i = 0; i < str.length(); i++)
  {
    int c = str.charAt(i);

    if (c != '.' && (c < '0' || c > '9'))
    {
      return false;
    }
  }

  return true;
}

/** IP to String? */
String toStringIp(IPAddress ip)
{
  String res = "";

  for (int i = 0; i < 3; i++)
  {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }

  res += String(((ip >> 8 * 3)) & 0xFF);

  return res;
}

/** Handle root or redirect to captive portal */
void handleRoot(AsyncWebServerRequest * request)
{
  if (captivePortal(request))
  {
    // If captive portal redirect instead of displaying the page.
    return;
  }

  String Page = F(
                  "<!DOCTYPE html><html lang='en'><head>"
                  "<meta name='viewport' content='width=device-width'>"
                  "<title>RP2040W-CaptivePortal</title></head><body>"
                  "<h1>HELLO WORLD!!</h1>");

  Page +=   "<h2>From " + String(BOARD_NAME) + " using CYW43439 WiFi</h2>";

  AsyncWebServerResponse *response = request->beginResponse(200, "text/html", Page);
  response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  response->addHeader("Pragma", "no-cache");
  response->addHeader("Expires", "-1");

  request->send(response);
}

// Redirect to captive portal if we got a request for another domain. 
// Return true in that case so the page handler do not try to handle the request again.
bool captivePortal(AsyncWebServerRequest * request)
{
  if (!isIp(request->host()))
  {
    Serial.println("Request redirected to captive portal");

    // Empty content inhibits Content-length header so we have to close the socket ourselves.
    AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
    response->addHeader("Location", String("http://") + toStringIp(request->client()->localIP()));

    request->send(response);

    request->client()->stop(); // Stop is needed because we sent no content length

    return true;
  }

  return false;
}

void handleNotFound(AsyncWebServerRequest * request)
{
  if (captivePortal(request))
  {
    // If captive portal redirect instead of displaying the error page.
    return;
  }

  String message = F("File Not Found\n\n");

  message += F("URI: ");
  message += request->url();
  message += F("\nMethod: ");
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += request->args();
  message += F("\n");

  for (uint8_t i = 0; i < request->args(); i++)
  {
    message += String(F(" ")) + request->argName(i) + F(": ") + request->arg(i) + F("\n");
  }

  AsyncWebServerResponse *response = request->beginResponse(404, "text/plain", message);
  response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  response->addHeader("Pragma", "no-cache");
  response->addHeader("Expires", "-1");

  request->send(response);
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

  Serial.print("\nStart AsyncCaptivePortalAdvanced on "); Serial.println(BOARD_NAME);
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

  dnsServer.start(DNS_PORT, "*", apIP);

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  // simple HTTP server to see that DNS server is working
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    handleRoot(request);
  });

  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    handleRoot(request);
  });

  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    handleRoot(request);
  });

  server.onNotFound(handleNotFound);

  server.begin(); // Web server start

  Serial.print(F("HTTP DNSServer is @ IP : "));
  Serial.println(apIP);
}

void loop()
{
}
