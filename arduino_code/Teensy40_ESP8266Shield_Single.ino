/****************************************************************************************************************************
  Teensy40_ESP8266Shield_Single.ino
  For Teensy using ESP8266 WiFi Shield
  
  Blynk_Esp8266AT_WM is a library for the Mega, Teensy, SAM DUE and SAMD boards (https://github.com/khoih-prog/Blynk_Esp8266AT_WM)
  to enable easy configuration/reconfiguration and autoconnect/autoreconnect of WiFi/Blynk
  
  Based on and Modified from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
  Built by Khoi Hoang https://github.com/khoih-prog/Blynk_Esp8266AT_WM
  Licensed under MIT license
 *****************************************************************************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#if !( defined(TEENSYDUINO) || defined(CORE_TEENSY) )
  #error This code is intended to run on Teensy platform! Please check your Tools->Board setting.
#endif

///////////////////////////////

// Debug level, 0-3
#define BLYNK_WM_DEBUG                3
#define SIMPLE_SHIELD_ESP8266_DEBUG   0

//#define USE_NEW_WEBSERVER_VERSION     true  //false

#define _ESP_AT_LOGLEVEL_             1
#define _ESP_AT_LIB_LOGLEVEL_         1

/* Comment this out to disable prints and save space */
#define ESP_AT_DEBUG_OUTPUT       Serial
//#define ESP_AT_LIB_DEBUG_OUTPUT   Serial

#define ESP_AT_DEBUG              true
#define ESP_AT_LIB_DEBUG          true

// Uncomment to use ESP32-AT commands
//#define USE_ESP32_AT      true

// USE_ESP_AT_LIB == true to use new ESP_AT_Lib, instead of ESP8266_Lib
// For ESP32-AT, must use ESP_AT_Lib
#if (defined(USE_ESP32_AT) && USE_ESP32_AT )
  #define USE_ESP_AT_LIB    true
#else
  #define USE_ESP_AT_LIB    true
  //#define USE_ESP_AT_LIB    false
#endif

///////////////////////////////

#ifdef CORE_TEENSY
  // For Teensy 4.1/4.0
  //#define EspSerial Serial1   //Serial1, Pin RX1 :  0, TX1 :  1
  #define EspSerial Serial2   //Serial2, Pin RX2 :  7, TX2 :  8
  //#define EspSerial Serial3   //Serial3, Pin RX3 : 15, TX3 : 14
  //#define EspSerial Serial4   //Serial4, Pin RX4 : 16, TX4 : 17
  
  #if defined(__IMXRT1062__)
    // For Teensy 4.1/4.0
    #define BOARD_TYPE      "TEENSY 4.1/4.0"
  #elif defined(__MK66FX1M0__)
    #define BOARD_TYPE "Teensy 3.6"
  #elif defined(__MK64FX512__)
    #define BOARD_TYPE "Teensy 3.5"
  #elif defined(__MKL26Z64__)
    #define BOARD_TYPE "Teensy LC"
  #elif defined(__MK20DX256__)
    #define BOARD_TYPE "Teensy 3.2" // and Teensy 3.1 (obsolete)
  #elif defined(__MK20DX128__)
    #define BOARD_TYPE "Teensy 3.0"
  #elif defined(__AVR_AT90USB1286__)
    #error Teensy 2.0++ not supported yet
  #elif defined(__AVR_ATmega32U4__)
    #error Teensy 2.0 not supported yet
  #else
    // For Other Boards
    #define BOARD_TYPE      "Unknown Teensy Board"
  #endif

#endif

#ifndef BOARD_NAME
  #define BOARD_NAME    BOARD_TYPE
#endif

//#include <ESP8266_Lib.h>

// Start location in EEPROM to store config data. Default 0
// Config data Size currently is 128 bytes)
#define EEPROM_START     128

#define USE_BLYNK_WM      true
//#define USE_BLYNK_WM      false

#if USE_BLYNK_WM
  // SSID and PW for Config Portal
  String portal_ssid      = "CfgPrtl-SSID";
  String portal_password  = "CfgPrtl-PW";
  
  #ifdef CORE_TEENSY
    #include <BlynkSimpleShieldEsp8266_Teensy_WM_Single.h>
  #else
    #include <BlynkSimpleShieldEsp8266_WM.h>
  #endif
#else
  #include <BlynkSimpleShieldEsp8266_Teensy.h>

  #define USE_LOCAL_SERVER      true
  
  #if USE_LOCAL_SERVER
    char auth[] = "****";
    String BlynkServer = "account.duckdns.org";
    //String BlynkServer = "192.168.2.112";
  #else
    char auth[] = "****";
    String BlynkServer = "blynk-cloud.com";
  #endif
  
  #define BLYNK_SERVER_HARDWARE_PORT    8080
  
  // Your WiFi credentials.
  char ssid[] = "****";
  char pass[] = "****";

#endif

// Your Teensy <-> ESP8266 baud rate:
#define ESP8266_BAUD 115200

ESP8266 wifi(&EspSerial);

#if USE_BLYNK_WM

#define BLYNK_PIN_FORCED_CONFIG           V10
#define BLYNK_PIN_FORCED_PERS_CONFIG      V20

// Use button V10 (BLYNK_PIN_FORCED_CONFIG) to forced Config Portal
BLYNK_WRITE(BLYNK_PIN_FORCED_CONFIG)
{
  if (param.asInt())
  {
    Serial.println( F("\nCP Button Hit. Rebooting") );

    // This will keep CP once, clear after reset, even you didn't enter CP at all.
    Blynk.resetAndEnterConfigPortal();
  }
}

// Use button V20 (BLYNK_PIN_FORCED_PERS_CONFIG) to forced Persistent Config Portal
BLYNK_WRITE(BLYNK_PIN_FORCED_PERS_CONFIG)
{
  if (param.asInt())
  {
    Serial.println( F("\nPersistent CP Button Hit. Rebooting") );

    // This will keep CP forever, until you successfully enter CP, and Save data to clear the flag.
    Blynk.resetAndEnterConfigPortalPersistent();
  }
}

#endif

void heartBeatPrint()
{
  static int num = 1;

  if (Blynk.connected())
  {
    Serial.print(F("B"));
  }
  else
  {
    Serial.print(F("F"));
  }

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(F(" "));
  }
}

void check_status()
{
  static unsigned long checkstatus_timeout = 0;

#define STATUS_CHECK_INTERVAL     15000L

  // Send status report every STATUS_REPORT_INTERVAL (60) seconds: we don't need to send updates frequently if there is no status change.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    // report status to Blynk
    heartBeatPrint();

    checkstatus_timeout = millis() + STATUS_CHECK_INTERVAL;
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial);
  
  delay(200);

  Serial.print(F("\nStart Teensy40_ESP8266Shield_Single on ")); Serial.println(BOARD_NAME);
  Serial.println(BLYNK_ESP8266AT_WM_VERSION);
  Serial.println(ESP_AT_LIB_VERSION);

  // initialize serial for ESP module
  EspSerial.begin(ESP8266_BAUD);

#if USE_BLYNK_WM
  Serial.println(DOUBLERESETDETECTOR_GENERIC_VERSION);
  Serial.println(F("Start Blynk_ESP8266AT_WM"));
  //Blynk.setConfigPortalIP(IPAddress(192, 168, 100, 1));
  Blynk.setConfigPortalChannel(0);
  Blynk.setConfigPortal(portal_ssid, portal_password);
  //Blynk.setConfigPortal("Teensy4", "MyTeensy4");
  Blynk.begin(wifi);
#else
  Serial.print(F("Start Blynk no WM with BlynkServer = "));
  Serial.print(BlynkServer);
  Serial.print(F(" and Token = "));
  Serial.println(auth);
  
  Blynk.begin(auth, wifi, ssid, pass, BlynkServer.c_str(), BLYNK_SERVER_HARDWARE_PORT);
#endif
}

void loop()
{
  Blynk.run();
  check_status();
}
