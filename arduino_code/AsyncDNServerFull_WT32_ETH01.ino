/****************************************************************************************************************************
  AsyncDNSServerFull_WT32_ETH01.ino

  AsyncDNSServer_WT32_ETH01 is a Async DNS Server library for WT32_ETH01 (ESP32 + LAN8720)

  Based on and modified from ESPAsyncDNSServer Library (https://github.com/devyte/ESPAsyncDNSServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncDNSServer_WT32_ETH01
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

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

const byte DNS_PORT = 53;

IPAddress apIP;

AsyncDNSServer dnsServer;

AsyncWebServer server(80);

void handleNotFound(AsyncWebServerRequest *request)
{
  String message = "Hello World from " + String(BOARD_NAME);
  message += "\nURI: ";
  message += request->url();

  request->send(200, "text/plain", message);
}

void setup()
{
  Serial.begin(115200);

  while (!Serial && (millis() < 5000));

  Serial.print("\nStarting AsyncDNSServerFull_WT32_ETH01 on " + String(ARDUINO_BOARD));
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

  // start DNS server for a specific domain name
  dnsServer.start(DNS_PORT, "*", apIP);

  // simple HTTP server to see that DNS server is working
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(200, "text/plain", "Hello from AsyncDNSServer running on " + String(BOARD_NAME));
  });

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.print(F("AsyncDNSServer is @ IP : "));
  Serial.println(apIP);
}

void loop()
{
}
