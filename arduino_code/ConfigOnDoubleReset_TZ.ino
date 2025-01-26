/****************************************************************************************************************************
  ConfigOnDoubleReset_TZ.ino
  For Ethernet shields using ESP32_W6100 (ESP32 + LwIP W6100)

  WebServer_ESP32_W6100 is a library for the ESP32 with Ethernet W6100 to run WebServer

  Modified from
  1. Tzapu               (https://github.com/tzapu/WiFiManager)
  2. Ken Taylor          (https://github.com/kentaylor)
  3. Khoi Hoang          (https://github.com/khoih-prog/ESP_WiFiManager)

  Built by Khoi Hoang https://github.com/khoih-prog/ESP32_W6100_Manager
  Licensed under MIT license
 *****************************************************************************************************************************/
/****************************************************************************************************************************
   This example will open a configuration portal when the reset button is pressed twice.
   This method works well on Wemos boards which have a single reset button on board. It avoids using a pin for launching the configuration portal.

   Settings
   There are two values to be set in the sketch.

   DRD_TIMEOUT - Number of seconds to wait for the second reset. Set to 10 in the example.
   DRD_ADDRESS - The address in ESP8266 RTC RAM to store the flag. This memory must not be used for other purposes in the same sketch. Set to 0 in the example.

   This example, originally relied on the Double Reset Detector library from https://github.com/datacute/DoubleResetDetector
   To support ESP32, use ESP_DoubleResetDetector library from //https://github.com/khoih-prog/ESP_DoubleResetDetector
 *****************************************************************************************************************************/

#if !( defined(ESP32) )
  #error This code is intended to run on the (ESP32 + LwIP W6100) platform! Please check your Tools->Board setting.
#endif

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _ESP32_ETH_MGR_LOGLEVEL_    4

// To not display stored SSIDs and PWDs on Config Portal, select false. Default is true
// Even the stored Credentials are not display, just leave them all blank to reconnect and reuse the stored Credentials
//#define DISPLAY_STORED_CREDENTIALS_IN_CP        false

//////////////////////////////////////////////////////////////

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
//IPAddress myIP(192, 168, 2, 232);
//IPAddress myGW(192, 168, 2, 1);
//IPAddress mySN(255, 255, 255, 0);

// Google DNS Server IP
//IPAddress myDNS(8, 8, 8, 8);

//////////////////////////////////////////////////////////

// Optional values to override default settings
// Don't change unless you know what you're doing
//#define ETH_SPI_HOST        SPI3_HOST
//#define SPI_CLOCK_MHZ       25

// Must connect INT to GPIOxx or not working
//#define INT_GPIO            4

//#define MISO_GPIO           19
//#define MOSI_GPIO           23
//#define SCK_GPIO            18
//#define CS_GPIO             5

//////////////////////////////////////////////////////////

#include <FS.h>

//Ported to ESP32
#include <esp_wifi.h>

// LittleFS has higher priority than SPIFFS
#if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
  #define USE_LITTLEFS    true
  #define USE_SPIFFS      false
#elif defined(ARDUINO_ESP32C3_DEV)
  // For core v1.0.6-, ESP32-C3 only supporting SPIFFS and EEPROM. To use v2.0.0+ for LittleFS
  #define USE_LITTLEFS          false
  #define USE_SPIFFS            true
#endif

#if USE_LITTLEFS
  // Use LittleFS
  #include "FS.h"

  // Check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
  //#if ( ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(2, 0, 0) )  //(ESP_ARDUINO_VERSION_MAJOR >= 2)
  #if ( defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2) )
    #if (_ESP32_ETH_MGR_LOGLEVEL_ > 3)
      #warning Using ESP32 Core 1.0.6 or 2.0.0+
    #endif

    // The library has been merged into esp32 core from release 1.0.6
    #include <LittleFS.h>       // https://github.com/espressif/arduino-esp32/tree/master/libraries/LittleFS

    FS* filesystem =      &LittleFS;
    #define FileFS        LittleFS
    #define FS_Name       "LittleFS"
  #else
    #if (_ESP32_ETH_MGR_LOGLEVEL_ > 3)
      #warning Using ESP32 Core 1.0.5-. You must install LITTLEFS library
    #endif

    // The library has been merged into esp32 core from release 1.0.6
    #include <LITTLEFS.h>       // https://github.com/lorol/LITTLEFS

    FS* filesystem =      &LITTLEFS;
    #define FileFS        LITTLEFS
    #define FS_Name       "LittleFS"
  #endif

#elif USE_SPIFFS
  #include <SPIFFS.h>
  FS* filesystem =      &SPIFFS;
  #define FileFS        SPIFFS
  #define FS_Name       "SPIFFS"
#else
  // +Use FFat
  #include <FFat.h>
  FS* filesystem =      &FFat;
  #define FileFS        FFat
  #define FS_Name       "FFat"
#endif

//////////////////////////////////////////////////////////////

#define LED_BUILTIN       2
#define LED_ON            HIGH
#define LED_OFF           LOW

//////////////////////////////////////////////////////////////

// These defines must be put before #include <ESP_DoubleResetDetector.h>
// to select where to store DoubleResetDetector's variable.
// For ESP32, You must select one to be true (EEPROM or SPIFFS)
// For ESP8266, You must select one to be true (RTC, EEPROM, SPIFFS or LITTLEFS)
// Otherwise, library will use default EEPROM storage

// These defines must be put before #include <ESP_DoubleResetDetector.h>
// to select where to store DoubleResetDetector's variable.
// For ESP32, You must select one to be true (EEPROM or SPIFFS)
// Otherwise, library will use default EEPROM storage
#if USE_LITTLEFS
  #define ESP_DRD_USE_LITTLEFS    true
  #define ESP_DRD_USE_SPIFFS      false
  #define ESP_DRD_USE_EEPROM      false
#elif USE_SPIFFS
  #define ESP_DRD_USE_LITTLEFS    false
  #define ESP_DRD_USE_SPIFFS      true
  #define ESP_DRD_USE_EEPROM      false
#else
  #define ESP_DRD_USE_LITTLEFS    false
  #define ESP_DRD_USE_SPIFFS      false
  #define ESP_DRD_USE_EEPROM      true
#endif

#define DOUBLERESETDETECTOR_DEBUG       true  //false

#include <ESP_DoubleResetDetector.h>      //https://github.com/khoih-prog/ESP_DoubleResetDetector

// Number of seconds after reset during which a
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

//DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);
DoubleResetDetector* drd;//////

// Onboard LED I/O pin on NodeMCU board
const int PIN_LED = 2; // D4 on NodeMCU and WeMos. GPIO2/ADC12 of ESP32. Controls the onboard LED.

// You only need to format the filesystem once
//#define FORMAT_FILESYSTEM       true
#define FORMAT_FILESYSTEM         false

//////////////////////////////////////////////////////////////

// Assuming max 49 chars
#define TZNAME_MAX_LEN            50
#define TIMEZONE_MAX_LEN          50

typedef struct
{
  char TZ_Name[TZNAME_MAX_LEN];     // "America/Toronto"
  char TZ[TIMEZONE_MAX_LEN];        // "EST5EDT,M3.2.0,M11.1.0"
  uint16_t checksum;
} EthConfig;

EthConfig         Ethconfig;

#define  CONFIG_FILENAME              F("/eth_cred.dat")

//////////////////////////////////////////////////////////////

// Indicates whether ESP has credentials saved from previous session, or double reset detected
bool initialConfig = false;

// Use false if you don't like to display Available Pages in Information Page of Config Portal
// Comment out or use true to display Available Pages in Information Page of Config Portal
// Must be placed before #include <ESP32_W6100_Manager.h>
#define USE_AVAILABLE_PAGES     true  //false

// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
//#define USE_STATIC_IP_CONFIG_IN_CP          false

// Use false to disable NTP config. Advisable when using Cellphone, Tablet to access Config Portal.
// See Issue 23: On Android phone ConfigPortal is unresponsive (https://github.com/khoih-prog/ESP_WiFiManager/issues/23)
#define USE_ESP_ETH_MANAGER_NTP     true

// Just use enough to save memory. On ESP8266, can cause blank ConfigPortal screen
// if using too much memory
#define USING_AFRICA        false
#define USING_AMERICA       true
#define USING_ANTARCTICA    false
#define USING_ASIA          false
#define USING_ATLANTIC      false
#define USING_AUSTRALIA     false
#define USING_EUROPE        false
#define USING_INDIAN        false
#define USING_PACIFIC       false
#define USING_ETC_GMT       false

// Use true to enable CloudFlare NTP service. System can hang if you don't have Internet access while accessing CloudFlare
// See Issue #21: CloudFlare link in the default portal (https://github.com/khoih-prog/ESP_WiFiManager/issues/21)
#define USE_CLOUDFLARE_NTP          false

// New in v1.0.11
#define USING_CORS_FEATURE          true

//////////////////////////////////////////////////////////////

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
  // Force DHCP to be true
  #if defined(USE_DHCP_IP)
    #undef USE_DHCP_IP
  #endif
  #define USE_DHCP_IP     true
#else
  // You can select DHCP or Static IP here
  #define USE_DHCP_IP     true
  //#define USE_DHCP_IP     false
#endif

#if ( USE_DHCP_IP )
  // Use DHCP

  #if (_ESP32_ETH_MGR_LOGLEVEL_ > 3)
    #warning Using DHCP IP
  #endif

  IPAddress stationIP   = IPAddress(0, 0, 0, 0);
  IPAddress gatewayIP   = IPAddress(192, 168, 2, 1);
  IPAddress netMask     = IPAddress(255, 255, 255, 0);

#else
  // Use static IP

  #if (_ESP32_ETH_MGR_LOGLEVEL_ > 3)
    #warning Using static IP
  #endif

  IPAddress stationIP   = IPAddress(192, 168, 2, 232);
  IPAddress gatewayIP   = IPAddress(192, 168, 2, 1);
  IPAddress netMask     = IPAddress(255, 255, 255, 0);
#endif

//////////////////////////////////////////////////////////////

#define USE_CONFIGURABLE_DNS      true

IPAddress dns1IP      = gatewayIP;
IPAddress dns2IP      = IPAddress(8, 8, 8, 8);

#include <ESP32_W6100_Manager.h>               //https://github.com/khoih-prog/ESP32_W6100_Manager

#define HTTP_PORT     80

//////////////////////////////////////////////////////////////

/******************************************
   // Defined in ESP32_W6100_Manager.hpp
  typedef struct
  {
    IPAddress _sta_static_ip;
    IPAddress _sta_static_gw;
    IPAddress _sta_static_sn;
    #if USE_CONFIGURABLE_DNS
    IPAddress _sta_static_dns1;
    IPAddress _sta_static_dns2;
    #endif
  }  ETH_STA_IPConfig;
******************************************/

ETH_STA_IPConfig EthSTA_IPconfig;

//////////////////////////////////////////////////////////////

void initSTAIPConfigStruct(ETH_STA_IPConfig &in_EthSTA_IPconfig)
{
  in_EthSTA_IPconfig._sta_static_ip   = stationIP;
  in_EthSTA_IPconfig._sta_static_gw   = gatewayIP;
  in_EthSTA_IPconfig._sta_static_sn   = netMask;
#if USE_CONFIGURABLE_DNS
  in_EthSTA_IPconfig._sta_static_dns1 = dns1IP;
  in_EthSTA_IPconfig._sta_static_dns2 = dns2IP;
#endif
}

//////////////////////////////////////////////////////////////

void displayIPConfigStruct(ETH_STA_IPConfig in_EthSTA_IPconfig)
{
  LOGERROR3(F("stationIP ="), in_EthSTA_IPconfig._sta_static_ip, ", gatewayIP =", in_EthSTA_IPconfig._sta_static_gw);
  LOGERROR1(F("netMask ="), in_EthSTA_IPconfig._sta_static_sn);
#if USE_CONFIGURABLE_DNS
  LOGERROR3(F("dns1IP ="), in_EthSTA_IPconfig._sta_static_dns1, ", dns2IP =", in_EthSTA_IPconfig._sta_static_dns2);
#endif
}

//////////////////////////////////////////////////////////////

#if USE_ESP_ETH_MANAGER_NTP
void printLocalTime()
{
  struct tm timeinfo;

  getLocalTime( &timeinfo );

  // Valid only if year > 2000.
  // You can get from timeinfo : tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec
  if (timeinfo.tm_year > 100 )
  {
    Serial.print("Local Date/Time: ");
    Serial.print( asctime( &timeinfo ) );
  }
}
#endif

//////////////////////////////////////////////////////////////

void heartBeatPrint()
{
#if USE_ESP_ETH_MANAGER_NTP
  printLocalTime();
#else
  static int num = 1;

  if (ESP32_W6100_isConnected())
    Serial.print(F("H"));        // H means connected to Ethernet
  else
    Serial.print(F("F"));        // F means not connected to Ethernet

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(F(" "));
  }

#endif
}

//////////////////////////////////////////////////////////////

void check_status()
{
  static ulong checkstatus_timeout  = 0;

  static ulong current_millis;

#if USE_ESP_ETH_MANAGER_NTP
#define HEARTBEAT_INTERVAL    60000L
#else
#define HEARTBEAT_INTERVAL    10000L
#endif

  current_millis = millis();

  // Print hearbeat every HEARTBEAT_INTERVAL (10) seconds.
  if ((current_millis > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    heartBeatPrint();
    checkstatus_timeout = current_millis + HEARTBEAT_INTERVAL;
  }
}

//////////////////////////////////////////////////////////////

int calcChecksum(uint8_t* address, uint16_t sizeToCalc)
{
  uint16_t checkSum = 0;

  for (uint16_t index = 0; index < sizeToCalc; index++)
  {
    checkSum += * ( ( (byte*) address ) + index);
  }

  return checkSum;
}

//////////////////////////////////////////////////////////////

bool loadConfigData()
{
  File file = FileFS.open(CONFIG_FILENAME, "r");
  LOGERROR(F("LoadCfgFile "));

  memset((void *) &Ethconfig,       0, sizeof(Ethconfig));
  memset((void *) &EthSTA_IPconfig, 0, sizeof(EthSTA_IPconfig));

  if (file)
  {
    file.readBytes((char *) &Ethconfig,   sizeof(Ethconfig));
    file.readBytes((char *) &EthSTA_IPconfig, sizeof(EthSTA_IPconfig));
    file.close();

    LOGERROR(F("OK"));

    if ( Ethconfig.checksum != calcChecksum( (uint8_t*) &Ethconfig, sizeof(Ethconfig) - sizeof(Ethconfig.checksum) ) )
    {
      LOGERROR(F("Ethconfig checksum wrong"));

      return false;
    }

    displayIPConfigStruct(EthSTA_IPconfig);

    return true;
  }
  else
  {
    LOGERROR(F("failed"));

    return false;
  }
}

//////////////////////////////////////////////////////////////

void saveConfigData()
{
  File file = FileFS.open(CONFIG_FILENAME, "w");
  LOGERROR(F("SaveCfgFile "));

  if (file)
  {
    Ethconfig.checksum = calcChecksum( (uint8_t*) &Ethconfig, sizeof(Ethconfig) - sizeof(Ethconfig.checksum) );

    file.write((uint8_t*) &Ethconfig, sizeof(Ethconfig));

    displayIPConfigStruct(EthSTA_IPconfig);

    file.write((uint8_t*) &EthSTA_IPconfig, sizeof(EthSTA_IPconfig));
    file.close();

    LOGERROR(F("OK"));
  }
  else
  {
    LOGERROR(F("failed"));
  }
}

//////////////////////////////////////////////////////////////

void beginEthernet()
{
  LOGWARN(F("Default SPI pinout:"));
  LOGWARN1(F("SPI_HOST:"), ETH_SPI_HOST);
  LOGWARN1(F("MOSI:"), MOSI_GPIO);
  LOGWARN1(F("MISO:"), MISO_GPIO);
  LOGWARN1(F("SCK:"),  SCK_GPIO);
  LOGWARN1(F("CS:"),   CS_GPIO);
  LOGWARN1(F("INT:"),  INT_GPIO);
  LOGWARN1(F("SPI Clock (MHz):"), SPI_CLOCK_MHZ);
  LOGWARN(F("========================="));

  ///////////////////////////////////

  // To be called before ETH.begin()
  ESP32_W6100_onEvent();

  // start the ethernet connection and the server:
  // Use DHCP dynamic IP and random mac
  uint16_t index = millis() % NUMBER_OF_MAC;

  //bool begin(int MISO_GPIO, int MOSI_GPIO, int SCLK_GPIO, int CS_GPIO, int INT_GPIO, int SPI_CLOCK_MHZ,
  //           int SPI_HOST, uint8_t *W6100_Mac = W6100_Default_Mac);
  //ETH.begin( MISO_GPIO, MOSI_GPIO, SCK_GPIO, CS_GPIO, INT_GPIO, SPI_CLOCK_MHZ, ETH_SPI_HOST );
  ETH.begin( MISO_GPIO, MOSI_GPIO, SCK_GPIO, CS_GPIO, INT_GPIO, SPI_CLOCK_MHZ, ETH_SPI_HOST, mac[index] );
}

//////////////////////////////////////////////////////////////

void initEthernet()
{
#if !( USE_DHCP_IP )
  displayIPConfigStruct(EthSTA_IPconfig);

  // Static IP, leave without this line to get IP via DHCP
  //bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
  //ETH.config(stationIP, gatewayIP, netMask, dns1IP, dns2IP);
  ETH.config(EthSTA_IPconfig._sta_static_ip, EthSTA_IPconfig._sta_static_gw, EthSTA_IPconfig._sta_static_sn,
             EthSTA_IPconfig._sta_static_dns1);
#endif

  ESP32_W6100_waitForConnect();
}

//////////////////////////////////////////////////////////////

void setup()
{
  // put your setup code here, to run once:
  // initialize the LED digital pin as an output.
  pinMode(PIN_LED, OUTPUT);

  Serial.begin(115200);

  while (!Serial && millis() < 5000);

  delay(200);

  Serial.print(F("\nStarting ConfigOnDoubleReset_TZ using "));
  Serial.print(FS_Name);
  Serial.print(F(" on "));
  Serial.print(ARDUINO_BOARD);
  Serial.print(F(" with "));
  Serial.println(SHIELD_TYPE);
  Serial.println(ESP32_W6100_MANAGER_VERSION);
  Serial.println(ESP_DOUBLE_RESET_DETECTOR_VERSION);

  Serial.setDebugOutput(false);

  if (FORMAT_FILESYSTEM)
    FileFS.format();

  // Format FileFS if not yet
#ifdef ESP32

  if (!FileFS.begin(true))
#else
  if (!FileFS.begin())
#endif
  {
#ifdef ESP8266
    FileFS.format();
#endif

    Serial.println(F("SPIFFS/LittleFS failed! Already tried formatting."));

    if (!FileFS.begin())
    {
      // prevents debug info from the library to hide err message.
      delay(100);

#if USE_LITTLEFS
      Serial.println(F("LittleFS failed!. Please use SPIFFS or EEPROM. Stay forever"));
#else
      Serial.println(F("SPIFFS failed!. Please use LittleFS or EEPROM. Stay forever"));
#endif

      while (true)
      {
        delay(1);
      }
    }
  }

  drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);

  unsigned long startedAt = millis();

  beginEthernet();

  initSTAIPConfigStruct(EthSTA_IPconfig);

  //Local intialization. Once its business is done, there is no need to keep it around
  // Use this to default DHCP hostname to ESP32-XXXXXX
  //ESP32_W6100_Manager ESP32_W6100_manager;
  // Use this to personalize DHCP hostname (RFC952 conformed)
  ESP32_W6100_Manager ESP32_W6100_manager("ConfigOnDoubleReset_TZ");

#if !USE_DHCP_IP
  // Set (static IP, Gateway, Subnetmask, DNS1 and DNS2) or (IP, Gateway, Subnetmask)
  ESP32_W6100_manager.setSTAStaticIPConfig(EthSTA_IPconfig);
#endif

#if USING_CORS_FEATURE
  ESP32_W6100_manager.setCORSHeader("Your Access-Control-Allow-Origin");
#endif

  if (loadConfigData())
  {
    //If no access point name has been previously entered disable timeout.
    ESP32_W6100_manager.setConfigPortalTimeout(120);

    Serial.println(F("Got stored Credentials. Timeout 120s for Config Portal"));

#if USE_ESP_ETH_MANAGER_NTP

    if ( strlen(Ethconfig.TZ_Name) > 0 )
    {
      LOGERROR3(F("Current TZ_Name ="), Ethconfig.TZ_Name, F(", TZ = "), Ethconfig.TZ);

      //configTzTime(Ethconfig.TZ, "pool.ntp.org" );
      configTzTime(Ethconfig.TZ, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
    }
    else
    {
      Serial.println(F("Current Timezone is not set. Enter Config Portal to set."));
    }

#endif
  }
  else
  {
    // Enter CP only if no stored Credentials on flash and file
    Serial.println(F("Open Config Portal without Timeout: No stored Credentials."));
    initialConfig = true;
  }

  //////////////////////////////////

  // Connect ETH now if using STA
  initEthernet();

  //////////////////////////////////

  if (drd->detectDoubleReset())
  {
    // DRD, disable timeout.
    ESP32_W6100_manager.setConfigPortalTimeout(0);

    Serial.println(F("Open Config Portal without Timeout: Double Reset Detected"));
    initialConfig = true;
  }

  if (initialConfig)
  {
    Serial.print(F("Starting configuration portal @ "));
    Serial.println(ETH.localIP());

    digitalWrite(LED_BUILTIN, LED_ON); // Turn led on as we are in configuration mode.

    //sets timeout in seconds until configuration portal gets turned off.
    //If not specified device will remain in configuration mode until
    //switched off via webserver or device is restarted.
    //ESP32_W6100_manager.setConfigPortalTimeout(600);

    // Starts an access point
    if (!ESP32_W6100_manager.startConfigPortal())
      Serial.println(F("Not connected to ETH network but continuing anyway."));
    else
    {
      Serial.println(F("ETH network connected...yeey :)"));
    }

#if USE_ESP_ETH_MANAGER_NTP
    String tempTZ   = ESP32_W6100_manager.getTimezoneName();

    if (strlen(tempTZ.c_str()) < sizeof(Ethconfig.TZ_Name) - 1)
      strcpy(Ethconfig.TZ_Name, tempTZ.c_str());
    else
      strncpy(Ethconfig.TZ_Name, tempTZ.c_str(), sizeof(Ethconfig.TZ_Name) - 1);

    const char * TZ_Result = ESP32_W6100_manager.getTZ(Ethconfig.TZ_Name);

    if (strlen(TZ_Result) < sizeof(Ethconfig.TZ) - 1)
      strcpy(Ethconfig.TZ, TZ_Result);
    else
      strncpy(Ethconfig.TZ, TZ_Result, sizeof(Ethconfig.TZ_Name) - 1);

    if ( strlen(Ethconfig.TZ_Name) > 0 )
    {
      LOGERROR3(F("Saving current TZ_Name ="), Ethconfig.TZ_Name, F(", TZ = "), Ethconfig.TZ);

      //configTzTime(Ethconfig.TZ, "pool.ntp.org" );
      configTzTime(Ethconfig.TZ, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
    }
    else
    {
      LOGERROR(F("Current Timezone Name is not set. Enter Config Portal to set."));
    }

#endif

    ESP32_W6100_manager.getSTAStaticIPConfig(EthSTA_IPconfig);

    saveConfigData();
  }

  digitalWrite(PIN_LED, LED_OFF); // Turn led off as we are not in configuration mode.

  startedAt = millis();

  Serial.print(F("After waiting "));
  Serial.print((float) (millis() - startedAt) / 1000);
  Serial.print(F(" secs more in setup(), connection result is "));

  if (ESP32_W6100_isConnected())
  {
    Serial.print(F("connected. Local IP: "));
    Serial.println(ETH.localIP());
  }
}

//////////////////////////////////////////////////////////////

void loop()
{
  // Call the double reset detector loop method every so often,
  // so that it can recognise when the timeout expires.
  // You can also call drd.stop() when you wish to no longer
  // consider the next reset as a double reset.
  drd->loop();

  // put your main code here, to run repeatedly
  check_status();
}
