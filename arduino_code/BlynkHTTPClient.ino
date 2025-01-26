/****************************************************************************************************************************
  BlynkHTTPClient.ino
  For STM32 running built-in Ethernet

  BlynkEthernet_STM32_WM is a library for the STM32 running built-in Ethernet, ENC28J60 or W5x00 Ethernet shields
  to enable easy configuration/reconfiguration and autoconnect/autoreconnect to Blynk
  Based on and modified from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
  Built by Khoi Hoang https://github.com/khoih-prog/BlynkEthernet_STM32_WM
  Licensed under MIT license
 *****************************************************************************************************************************/

#include "defines.h"
#include "Credentials.h"
#include "dynamicParams.h"

// Default heartbeat interval for GSM is 60
// If you want override this value, uncomment and set this option:
//#define BLYNK_HEARTBEAT 30

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

#if ( USE_BUILTIN_ETHERNET || USE_UIP_ETHERNET || USE_ETHERNET2 || USE_ETHERNET3 || \
      USE_ETHERNET_LARGE || USE_ETHERNET_ENC || USE_ETHERNET || USE_CUSTOM_ETHERNET )
  #define BLYNK_HTTP_CLIENT_USING_ETHERNET    true
  EthernetClient client;
#else
  // Select your modem:
  #define TINY_GSM_MODEM_SIM800
  // #define TINY_GSM_MODEM_SIM808
  // #define TINY_GSM_MODEM_SIM900
  // #define TINY_GSM_MODEM_UBLOX
  // #define TINY_GSM_MODEM_BG96
  // #define TINY_GSM_MODEM_A6
  // #define TINY_GSM_MODEM_A7
  // #define TINY_GSM_MODEM_M590
  // #define TINY_GSM_MODEM_ESP8266
  // #define TINY_GSM_MODEM_XBEE
  
  // Increase RX buffer if needed
  //#define TINY_GSM_RX_BUFFER 512
  
  #include <TinyGsmClient.h>
  //#include <BlynkSimpleSIM800.h>
  #include <BlynkSimpleTinyGSM.h>
  
  // Hardware Serial on Mega, Leonardo, Micro
  //#define SerialAT Serial1
  #define SerialAT SERIAL_PORT_HARDWARE
  
  // Your GPRS credentials
  // Leave empty, if missing user or pass
  #define apn     "YourAPN"
  #define user   "****"
  #define pass    "****"
  
  #define blynk_gsm_auth     "****"
  
  // Uncomment this if you want to see all AT commands
  //#define DUMP_AT_COMMANDS
  
  #ifdef DUMP_AT_COMMANDS
    #include <StreamDebugger.h>
    StreamDebugger debugger(SerialAT, SerialMon);
    TinyGsm modem(debugger);
  #else
    TinyGsm modem(SerialAT);
  #endif
  
  TinyGsmClient client(modem);
#endif

#include <ArduinoHttpClient.h>

// Server details. Currently hardcoded.
//http://account.duckdns.org
//#define http_server     "account.duckdns.org"
//#define resource        "/"

#define http_server     "vsh.pp.ua"
#define resource        "/TinyGSM/logo.txt"

#define http_port       80

HttpClient http(client, http_server, http_port);

void setup()
{
  // Set console baud rate
  SerialMon.begin(115200);
  delay(200);

#if ( BLYNK_HTTP_CLIENT_USING_ETHERNET )  
  Serial.print(F("\nStart BlynkHTTPClient on ")); Serial.print(BOARD_NAME);
  Serial.print(F(" using ")); Serial.println(SHIELD_TYPE);
  Serial.println(BLYNK_ETHERNET_STM32_WM_VERSION);

  #if !(USE_BUILTIN_ETHERNET || USE_UIP_ETHERNET)
    // Deselect the SD card
    pinMode(SDCARD_CS, OUTPUT);
    digitalWrite(SDCARD_CS, HIGH);
  #endif

#else
  Serial.print(F("\nStart BlynkHTTPClient on ")); Serial.print(BOARD_NAME);
  Serial.println(F(" using GSM/GPRS Modem");

  // Set GSM module baud rate
  SerialAT.begin(115200);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println(F("Initializing modem..."));
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print(F("Modem: "));
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");
#endif

#if ( BLYNK_HTTP_CLIENT_USING_ETHERNET )
#if USE_BLYNK_WM
  Blynk.begin();
#else
#if USE_LOCAL_SERVER
  Blynk.begin(blynk_auth, blynk_server, BLYNK_HARDWARE_PORT);
#else
  Blynk.begin(blynk_auth);
#endif
#endif
#else
  Blynk.begin(blynk_gsm_auth, modem, apn, user, pass);
#endif

  if (Blynk.connected())
    SerialMon.println(F("Blynk connected"));
}

void HTTPClientHandle()
{
#if !( BLYNK_HTTP_CLIENT_USING_ETHERNET )

  SerialMon.print(F("Waiting for network..."));
  if (!modem.waitForNetwork())
  {
    SerialMon.println(F(" fail"));
    return;
  }

  SerialMon.println(F(" OK"));

  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, user, pass))
  {
    SerialMon.println(F(" fail"));
    return;
  }
  SerialMon.println(F(" OK"));
#endif

#define USE_DIRECT_CLIENT     true

#if USE_DIRECT_CLIENT

  // Test client directly, see Arduino_TinyGSM.ino
  // https://github.com/Xinyuan-LilyGO/TTGO-T-Call/blob/master/examples/Arduino_TinyGSM/Arduino_TinyGSM.ino
  // Just added
  SerialMon.print(F("Connecting to "));
  SerialMon.print(http_server);
  if (!client.connect(http_server, http_port))
  {
    SerialMon.println(F(" fail"));
    //delay(10000);
    return;
  }
  SerialMon.println(F(" OK"));

  // Make a HTTP GET request:
  SerialMon.println(F("Performing HTTP GET request..."));
  client.print(String(F("GET ")) + resource + F(" HTTP/1.1\r\n"));
  client.print(String(F("Host: ")) + http_server + F("\r\n"));
  client.print(F("Connection: close\r\n\r\n"));
  client.println();

  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 10000L)
  {
    // Print available data
    while (client.available())
    {
      char c = client.read();
      SerialMon.print(c);
      timeout = millis();
    }
  }
  SerialMon.println();

  // Shutdown

  client.stop();

#else

  SerialMon.print(F("Performing HTTP GET request... "));
  int err = http.get(resource);
  
  if (err != 0)
  {
    SerialMon.print(F("failed to connect, error = "));
    SerialMon.println(err);
    return;
  }

  int status = http.responseStatusCode();
  SerialMon.println(status);
  
  if (!status)
  {
    SerialMon.print(F("Status Code is: "));
    SerialMon.println(status);
    return;
  }

  while (http.headerAvailable())
  {
    String headerName = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    //SerialMon.println(headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  
  if (length >= 0)
  {
    SerialMon.print(F("Content length is: "));
    SerialMon.println(length);
  }

  if (http.isResponseChunked())
  {
    SerialMon.println(F("The response is chunked"));
  }

  String body = http.responseBody();
  SerialMon.println(F("Response:"));
  SerialMon.println(body);

  SerialMon.print(F("Body length is: "));
  SerialMon.println(body.length());

  // Shutdown

  http.stop();
#endif

  SerialMon.println(F("Server disconnected"));

#if !( BLYNK_HTTP_CLIENT_USING_ETHERNET )
  modem.gprsDisconnect();
  SerialMon.println(F("GPRS disconnected"));
#endif
}

void check_status()
{
  static unsigned long checkstatus_timeout = 0;

#define STATUS_CHECK_INTERVAL     60000L

  // Send status report every STATUS_REPORT_INTERVAL (60) seconds: we don't need to send updates frequently if there is no status change.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    HTTPClientHandle();
    checkstatus_timeout = millis() + STATUS_CHECK_INTERVAL;
  }
}

#if (USE_BLYNK_WM && USE_DYNAMIC_PARAMETERS)
void displayCredentials()
{
  Serial.println(F("\nYour stored Credentials :"));

  for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
  {
    Serial.print(myMenuItems[i].displayName);
    Serial.print(F(" = "));
    Serial.println(myMenuItems[i].pdata);
  }
}
#endif

void loop()
{
  Blynk.run();
  check_status();

#if (USE_BLYNK_WM && USE_DYNAMIC_PARAMETERS)
  static bool displayedCredentials = false;

  if (!displayedCredentials)
  {
    for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
    {
      if (!strlen(myMenuItems[i].pdata))
      {
        break;
      }

      if ( i == (NUM_MENU_ITEMS - 1) )
      {
        displayedCredentials = true;
        displayCredentials();
      }
    }
  }
#endif    
}
