/****************************************************************************************************************************
  AsyncCaptivePortal_ESP32_ENC.ino

  For ESP32_Ethernet (ESP32/S2/S3/C3 + LwIP W6100 / W6100 / ENC28J60)

  AsyncDNSServer_ESP32_Ethernet is a library for the ESP32_Ethernet (ESP32/S2/S3/C3 + LwIP W6100 / W6100 / ENC28J60)

  Based on and modified from ESPAsyncDNSServer Library (https://github.com/devyte/ESPAsyncDNSServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncDNSServer_ESP32_Ethernet
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

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

const byte DNS_PORT = 53;

IPAddress apIP;

AsyncDNSServer dnsServer;
AsyncWebServer server(80);

String responseHTML = ""
                      "<!DOCTYPE html><html lang='en'><head>"
                      "<meta name='viewport' content='width=device-width'>"
                      "<title>ESP32_ENC-CaptivePortal</title></head><body>"
                      "<h1>Hello World from ESP32_ENC!</h1><p>This is a captive portal example."
                      " All requests will be redirected here.</p></body></html>";

void handleNotFound(AsyncWebServerRequest *request)
{
  request->send(200, "text/html", responseHTML);
}

void setup()
{
  Serial.begin(115200);

  while (!Serial && (millis() < 5000));

  Serial.print(F("\nStart AsyncCaptivePortal_ESP32_ENC on "));
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

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.print(F("AsyncDNSServer is @ IP : "));
  Serial.println(apIP);
}

void loop()
{
}
