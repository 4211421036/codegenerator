/****************************************************************************************************************************
  AsyncCaptivePortal_WT32_ETH01.ino

  AsyncDNSServer_WT32_ETH01 is a Async DNS Server library for WT32_ETH01 (ESP32 + LAN8720)

  Based on and modified from ESPAsyncDNSServer Library (https://github.com/devyte/ESPAsyncDNSServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncDNSServer_WT32_ETH01
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

#define ASYNC_DNS_WT32_ETH01_DEBUG_PORT      Serial

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _ASYNC_DNS_WT32_ETH01_LOGLEVEL_      1

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

const byte DNS_PORT = 53;

IPAddress apIP;

AsyncDNSServer dnsServer;
AsyncWebServer server(80);

String responseHTML = ""
                      "<!DOCTYPE html><html lang='en'><head>"
                      "<meta name='viewport' content='width=device-width'>"
                      "<title>WT32_ETH01-CaptivePortal</title></head><body>"
                      "<h1>Hello World from WT32_ETH01!</h1><p>This is a captive portal example."
                      " All requests will be redirected here.</p></body></html>";

void handleNotFound(AsyncWebServerRequest *request)
{
  request->send(200, "text/html", responseHTML);
}

void setup()
{
  Serial.begin(115200);

  while (!Serial && (millis() < 5000));

  Serial.print("\nStarting AsyncCaptivePortal_WT32_ETH01 on " + String(ARDUINO_BOARD));
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
