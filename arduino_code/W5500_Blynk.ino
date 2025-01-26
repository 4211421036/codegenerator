/****************************************************************************************************************************
  W5500_Blynk.ino
  For ESP32, ESP8266, Teensy, SAMD, SAM DUE using W5x00 Ethernet shields
  
  BlynkEthernet_WM is a library for Teensy, ESP, SAM DUE and SAMD boards, with Ethernet W5X00 or ENC28J60 shields,
  to enable easy configuration/reconfiguration and autoconnect/autoreconnect of Ethernet/Blynk
  AVR Mega and W5100 is not supported.
  Library modified from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
  Built by Khoi Hoang https://github.com/khoih-prog/BlynkEthernet_WM
  Licensed under MIT license
 *****************************************************************************************************************************/

#include <SPI.h>

#include "defines.h"
#include "Credentials.h"

#if USE_BLYNK_WM
  
  #include "dynamicParams.h"

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

void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial);

  delay(200);

#if ( USE_LITTLEFS || USE_SPIFFS)
  Serial.print(F("\nStart W5500_Blynk using ")); Serial.print(CurrentFileFS);
  Serial.print(F(" on ")); Serial.print(BOARD_NAME);
#else
  Serial.print(F("\nStart W5500_Blynk on ")); Serial.print(BOARD_NAME);
#endif

  Serial.print(F(" with ")); Serial.println(SHIELD_TYPE);

#if USE_BLYNK_WM 
  Serial.println(BLYNK_ETHERNET_WM_VERSION);
#endif

  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH); // Deselect the SD card

  #if USE_ETHERNET_WRAPPER

    EthernetInit();

  #else

    #if USE_ETHERNET
      Serial.println(F("=========== USE_ETHERNET ==========="));
    #elif USE_ETHERNET2
      Serial.println(F("=========== USE_ETHERNET2 ==========="));
    #elif USE_ETHERNET3
      Serial.println(F("=========== USE_ETHERNET3 ==========="));
    #elif USE_ETHERNET_LARGE
      Serial.println(F("=========== USE_ETHERNET_LARGE ==========="));
    #elif USE_ETHERNET_ESP8266
      Serial.println(F("=========== USE_ETHERNET_ESP8266 ==========="));
    #else
      Serial.println(F("========================="));
    #endif
   
      Serial.println(F("Default SPI pinout:"));
      Serial.print(F("MOSI:")); Serial.println(MOSI);
      Serial.print(F("MISO:")); Serial.println(MISO);
      Serial.print(F("SCK:"));  Serial.println(SCK);
      Serial.print(F("SS:"));   Serial.println(SS);
      Serial.println(F("========================="));
       
    #if defined(ESP8266)
      // For ESP8266, change for other boards if necessary
      #ifndef USE_THIS_SS_PIN
        #define USE_THIS_SS_PIN   D2    // For ESP8266
      #endif
      
      Serial.print(F("ESP8266 setCsPin::")); Serial.println(USE_THIS_SS_PIN);
      
      #if ( USE_ETHERNET || USE_ETHERNET_LARGE || USE_ETHERNET2 )
        // For ESP8266
        // Pin                D0(GPIO16)    D1(GPIO5)    D2(GPIO4)    D3(GPIO0)    D4(GPIO2)    D8
        // Ethernet           0                 X            X            X            X        0
        // Ethernet2          X                 X            X            X            X        0
        // Ethernet3          X                 X            X            X            X        0
        // EthernetLarge      X                 X            X            X            X        0
        // Ethernet_ESP8266   0                 0            0            0            0        0
        // D2 is safe to used for Ethernet, Ethernet2, Ethernet3, EthernetLarge libs
        // Must use library patch for Ethernet, EthernetLarge libraries
        //Ethernet.setCsPin (USE_THIS_SS_PIN);
        Ethernet.init (USE_THIS_SS_PIN);
  
      #elif USE_ETHERNET3
        // Use  MAX_SOCK_NUM = 4 for 4K, 2 for 8K, 1 for 16K RX/TX buffer
        #ifndef ETHERNET3_MAX_SOCK_NUM
          #define ETHERNET3_MAX_SOCK_NUM      4
        #endif
        
        Ethernet.setCsPin (USE_THIS_SS_PIN);
        Ethernet.init (ETHERNET3_MAX_SOCK_NUM);
 
      #endif  //( USE_ETHERNET || USE_ETHERNET2 || USE_ETHERNET3 || USE_ETHERNET_LARGE )
        
    #elif defined(ESP32)
  
      // You can use Ethernet.init(pin) to configure the CS pin
      //Ethernet.init(10);  // Most Arduino shields
      //Ethernet.init(5);   // MKR ETH shield
      //Ethernet.init(0);   // Teensy 2.0
      //Ethernet.init(20);  // Teensy++ 2.0
      //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
      //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet
      
      #ifndef USE_THIS_SS_PIN
        #define USE_THIS_SS_PIN   22    // For ESP32
      #endif
      
      Serial.print(F("ESP32 setCsPin::")); Serial.println(USE_THIS_SS_PIN);
      
      // For other boards, to change if necessary
      #if ( USE_ETHERNET || USE_ETHERNET_LARGE || USE_ETHERNET2 )
        // Must use library patch for Ethernet, EthernetLarge libraries
        // ESP32 => GPIO2,4,5,13,15,21,22 OK with Ethernet, Ethernet2, EthernetLarge
        // ESP32 => GPIO2,4,5,15,21,22 OK with Ethernet3
           
        //Ethernet.setCsPin (USE_THIS_SS_PIN);
        Ethernet.init (USE_THIS_SS_PIN);
  
      #elif USE_ETHERNET3
        // Use  MAX_SOCK_NUM = 4 for 4K, 2 for 8K, 1 for 16K RX/TX buffer
        #ifndef ETHERNET3_MAX_SOCK_NUM
          #define ETHERNET3_MAX_SOCK_NUM      4
        #endif
        
        Ethernet.setCsPin (USE_THIS_SS_PIN);
        Ethernet.init (ETHERNET3_MAX_SOCK_NUM);
              
      #endif  //( USE_ETHERNET || USE_ETHERNET2 || USE_ETHERNET3 || USE_ETHERNET_LARGE )
  
    #else   //defined(ESP8266)
      // unknown board, do nothing, use default SS = 10
      #ifndef USE_THIS_SS_PIN
        #define USE_THIS_SS_PIN   10    // For other boards
      #endif
           
      Serial.print(BOARD_NAME); Serial.print(F(" setCsPin::")); Serial.println(USE_THIS_SS_PIN);
  
      // For other boards, to change if necessary
      #if ( USE_ETHERNET || USE_ETHERNET_LARGE || USE_ETHERNET2 )
        // Must use library patch for Ethernet, Ethernet2, EthernetLarge libraries
  
        Ethernet.init (USE_THIS_SS_PIN);
  
      #elif USE_ETHERNET3
        // Use  MAX_SOCK_NUM = 4 for 4K, 2 for 8K, 1 for 16K RX/TX buffer
        #ifndef ETHERNET3_MAX_SOCK_NUM
          #define ETHERNET3_MAX_SOCK_NUM      4
        #endif
        
        Ethernet.setCsPin (USE_THIS_SS_PIN);
        Ethernet.init (ETHERNET3_MAX_SOCK_NUM);
                        
      #endif  //( USE_ETHERNET || USE_ETHERNET2 || USE_ETHERNET3 || USE_ETHERNET_LARGE )
      
    #endif    //defined(ESP8266)
  
  
  #endif  //USE_ETHERNET_WRAPPER

#if USE_BLYNK_WM
  Blynk.begin();
#else
#if USE_LOCAL_SERVER
  Blynk.begin(auth, server, BLYNK_SERVER_HARDWARE_PORT);
#else
  //Blynk.begin(auth);
  // You can also specify server:
  Blynk.begin(auth, server, BLYNK_SERVER_HARDWARE_PORT);
#endif
#endif

  if (Blynk.connected())
  {
#if USE_BLYNK_WM    
    Serial.print(F("Conn2Blynk: server = "));
    Serial.print(Blynk.getServerName());
    Serial.print(F(", port = "));
    Serial.println(Blynk.getHWPort());
    Serial.print(F("Token = "));
    Serial.print(Blynk.getToken());
   
#else
    Serial.print(F("Conn2Blynk: server = "));
    Serial.print(server);
    Serial.print(F(", port = "));
    Serial.println(BLYNK_SERVER_HARDWARE_PORT);
    Serial.print(F("Token = "));
    Serial.print(auth);   
#endif
    Serial.print(F(", IP = "));
    Serial.println(Ethernet.localIP());
  }
}

void heartBeatPrint()
{
  static int num = 1;

  if (Blynk.connected())
    Serial.print(F("B"));
  else
    Serial.print(F("F"));

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

#define STATUS_CHECK_INTERVAL     60000L

  // Send status report every STATUS_REPORT_INTERVAL (60) seconds: we don't need to send updates frequently if there is no status change.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    heartBeatPrint();
    checkstatus_timeout = millis() + STATUS_CHECK_INTERVAL;
  }
}

#if (USE_BLYNK_WM && USE_DYNAMIC_PARAMETERS)
void displayCredentials()
{
  Serial.println("\nYour stored Credentials :");

  for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
  {
    Serial.println(String(myMenuItems[i].displayName) + " = " + myMenuItems[i].pdata);
  }
}

void displayCredentialsInLoop()
{
  static bool displayedCredentials = false;

  if (!displayedCredentials)
  {
    for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
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
}
#endif

void loop()
{
  Blynk.run();
  check_status();

#if (USE_BLYNK_WM && USE_DYNAMIC_PARAMETERS)
  displayCredentialsInLoop();
#endif  
}
