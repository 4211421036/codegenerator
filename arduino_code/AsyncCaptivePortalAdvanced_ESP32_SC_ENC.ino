/****************************************************************************************************************************
  AsyncCaptivePortalAdvanced_ESP32_ENC.ino

  For ESP32_Ethernet (ESP32/S2/S3/C3 + LwIP W6100 / W6100 / ENC28J60)

  AsyncDNSServer_ESP32_Ethernet is a library for the ESP32_Ethernet (ESP32/S2/S3/C3 + LwIP W6100 / W6100 / ENC28J60)

  Based on and modified from ESPAsyncDNSServer Library (https://github.com/devyte/ESPAsyncDNSServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

/*
   This example serves a "hello world".

   Now the ESP32_ENC is in your network. You can reach it through http://192.168.x.x/

   This is a captive portal because it will redirect any http request to http://192.168.x.x/
*/

#if !( defined(ESP32) )
  #error This code is designed for (ESP32 + LwIP ENC28J60) to run on ESP32 platform! Please check your Tools->Board setting.
#endif

#define USING_W5500           false
#define USING_W6100           false
#define USING_ENC28J60        true

#define ASYNC_DNS_ESP32_ETHERNET_DEBUG_PORT      Serial

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _ASYNC_DNS_ESP32_ETHERNET_LOGLEVEL_      4

/////////////////////////////////////////////

// Enter a MAC address and IP address for your controller below.
#define NUMBER_OF_MAC      20

byte mac[][NUMBER_OF_MAC] =
{
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x02 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x03 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x04 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x05 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x06 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x07 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x08 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x09 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0A },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0B },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0C },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0E },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0F },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x10 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x11 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x12 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x13 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x14 },
};

// Select the IP address according to your local network
IPAddress myIP(192, 168, 2, 232);
IPAddress myGW(192, 168, 2, 1);
IPAddress mySN(255, 255, 255, 0);

// Google DNS Server IP
IPAddress myDNS(8, 8, 8, 8);

/////////////////////////////////////////////

// For ESP32-S3
// Optional values to override default settings
//#define ETH_SPI_HOST        SPI2_HOST
//#define SPI_CLOCK_MHZ       8

// Must connect INT to GPIOxx or not working
//#define INT_GPIO            4

//#define MISO_GPIO           13
//#define MOSI_GPIO           11
//#define SCK_GPIO            12
//#define CS_GPIO             10

// For ESP32_C3
// Optional values to override default settings
// Don't change unless you know what you're doing
//#define ETH_SPI_HOST        SPI2_HOST
//#define SPI_CLOCK_MHZ       8

// Must connect INT to GPIOxx or not working
//#define INT_GPIO            10

//#define MISO_GPIO           5
//#define MOSI_GPIO           6
//#define SCK_GPIO            4
//#define CS_GPIO             7

//////////////////////////////////////////////////////////

#include <AsyncDNSServer_ESP32_Ethernet.h>
#include <ESPAsyncWebServer.h>

//////////////////////////////////////////////////////////

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
                  "<title>ESP32_SC_SNC-CaptivePortal</title></head><body>"
                  "<h1>HELLO WORLD!!</h1>");

  Page +=   "<h2>From " + String(BOARD_NAME) + " using ENC28J60</h2>";

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

  delay(500);

  Serial.print(F("\nStart AsyncCaptivePortalAdvanced_ESP32_SC_ENC on "));
  Serial.print(ARDUINO_BOARD);
  Serial.print(F(" with "));
  Serial.println(SHIELD_TYPE);
  Serial.println(WEBSERVER_ESP32_SC_ENC_VERSION);
  Serial.println(ASYNC_UDP_ESP32_SC_ETHERNET_VERSION);
  Serial.println(ASYNC_DNS_SERVER_ESP32_ETHERNET_VERSION);

  Serial.setDebugOutput(true);

  DNS_LOGWARN(F("Default SPI pinout:"));
  DNS_LOGWARN1(F("SPI_HOST:"), ETH_SPI_HOST);
  DNS_LOGWARN1(F("MOSI:"), MOSI_GPIO);
  DNS_LOGWARN1(F("MISO:"), MISO_GPIO);
  DNS_LOGWARN1(F("SCK:"),  SCK_GPIO);
  DNS_LOGWARN1(F("CS:"),   CS_GPIO);
  DNS_LOGWARN1(F("INT:"),  INT_GPIO);
  DNS_LOGWARN1(F("SPI Clock (MHz):"), SPI_CLOCK_MHZ);
  DNS_LOGWARN(F("========================="));

  ///////////////////////////////////

  // To be called before ETH.begin()
  ESP32_ENC_onEvent();

  // start the ethernet connection and the server:
  // Use DHCP dynamic IP and random mac
  //bool begin(int MISO_GPIO, int MOSI_GPIO, int SCLK_GPIO, int CS_GPIO, int INT_GPIO, int SPI_CLOCK_MHZ,
  //           int SPI_HOST, uint8_t *W6100_Mac = W6100_Default_Mac);
  ETH.begin( MISO_GPIO, MOSI_GPIO, SCK_GPIO, CS_GPIO, INT_GPIO, SPI_CLOCK_MHZ, ETH_SPI_HOST );
  //ETH.begin( MISO_GPIO, MOSI_GPIO, SCK_GPIO, CS_GPIO, INT_GPIO, SPI_CLOCK_MHZ, ETH_SPI_HOST, mac[millis() % NUMBER_OF_MAC] );

  // Static IP, leave without this line to get IP via DHCP
  //bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
  //ETH.config(myIP, myGW, mySN, myDNS);

  ESP32_ENC_waitForConnect();

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
