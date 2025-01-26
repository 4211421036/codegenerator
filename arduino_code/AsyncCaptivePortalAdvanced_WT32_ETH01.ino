/****************************************************************************************************************************
  AsyncCaptivePortalAdvanced_WT32_ETH01.ino

  AsyncDNSServer_WT32_ETH01 is a Async DNS Server library for WT32_ETH01 (ESP32 + LAN8720)

  Based on and modified from ESPAsyncDNSServer Library (https://github.com/devyte/ESPAsyncDNSServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncDNSServer_WT32_ETH01
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

/*
   This example serves a "hello world".

   Now the WT32_ETH01 is in your network. You can reach it through http://192.168.x.x/

   This is a captive portal because it will redirect any http request to http://192.168.x.x/
*/

#define ASYNC_DNS_WT32_ETH01_DEBUG_PORT      Serial

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _ASYNC_DNS_WT32_ETH01_LOGLEVEL_      4

/////////////////////////////////////////////

// Select the IP address according to your local network
IPAddress myIP(192, 168, 2, 232);
IPAddress myGW(192, 168, 2, 1);
IPAddress mySN(255, 255, 255, 0);

// Google DNS Server IP
IPAddress myDNS(8, 8, 8, 8);

/////////////////////////////////////////////

#include <AsyncDNSServer_WT32_ETH01.h>
#include <ESPAsyncWebServer.h>


// DNS server
const byte DNS_PORT = 53;

AsyncDNSServer dnsServer;

// Web server
AsyncWebServer server(80);

IPAddress apIP;

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
                  "<title>WT32_ETH01-CaptivePortal</title></head><body>"
                  "<h1>HELLO WORLD!!</h1>");

  Page +=   "<h2>From " + String(BOARD_NAME) + " using LAN8720A</h2>";

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

void setup()
{
  Serial.begin(115200);

  while (!Serial && (millis() < 5000));

  Serial.print("\nStarting AsyncCaptivePortalAdvanced_WT32_ETH01 on " + String(ARDUINO_BOARD));
  Serial.println(" with " + String(SHIELD_TYPE));
  Serial.println(WEBSERVER_WT32_ETH01_VERSION);
  Serial.println(ASYNC_UDP_WT32_ETH01_VERSION);
  Serial.println(ASYNC_DNS_SERVER_WT32_ETH01_VERSION);

  Serial.setDebugOutput(true);

  ///////////////////////////////////

  /// To be called before ETH.begin()
  WT32_ETH01_onEvent();

  //bool begin(uint8_t phy_addr=ETH_PHY_ADDR, int power=ETH_PHY_POWER, int mdc=ETH_PHY_MDC, int mdio=ETH_PHY_MDIO,
  //           eth_phy_type_t type=ETH_PHY_TYPE, eth_clock_mode_t clk_mode=ETH_CLK_MODE);
  //ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);
  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);

  // Static IP, leave without this line to get IP via DHCP
  //bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
  ETH.config(myIP, myGW, mySN, myDNS);

  WT32_ETH01_waitForConnect();

  apIP = ETH.localIP();

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

  Serial.print(F("AsyncDNSServer is @ IP : "));
  Serial.println(apIP);
}

void loop()
{
}
