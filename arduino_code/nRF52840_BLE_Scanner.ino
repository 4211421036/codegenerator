/****************************************************************************************************************************
  BlynkEthernet_NRF52_WM.h
  For NRF52 or NRF52-based boards running W5x00, ENC28J60 Ethernet shields
  
  BlynkEthernet_WM is a library for Teensy, STM32, SAM DUE and SAMD boards, with Ethernet W5x00, ENC28J60 or NativeEthernet shields,
  to enable easy configuration/reconfiguration and autoconnect/autoreconnect of Ethernet/Blynk.
  AVR Mega is not supported.
  Library modified from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
  Built by Khoi Hoang https://github.com/khoih-prog/BlynkEthernet_WM
  Licensed under MIT license
*****************************************************************************************************************************/

#include <bluefruit.h>

#define BLYNK_PRINT Serial

#if ( defined(NRF52840_FEATHER) || defined(NRF52832_FEATHER) || defined(NRF52_SERIES) || defined(ARDUINO_NRF52_ADAFRUIT) || \
        defined(NRF52840_FEATHER_SENSE) || defined(NRF52840_ITSYBITSY) || defined(NRF52840_CIRCUITPLAY) || defined(NRF52840_CLUE) || \
        defined(NRF52840_METRO) || defined(NRF52840_PCA10056) || defined(PARTICLE_XENON) || defined(NINA_B302_ublox) || defined(NINA_B112_ublox) )
  #if defined(ETHERNET_USE_NRF52)
    #undef ETHERNET_USE_NRF528XX
  #endif
  #define ETHERNET_USE_NRF528XX         true
  #define USE_DYNAMIC_PARAMETERS        false //true
#endif

#if defined(ETHERNET_USE_NRF528XX)
  #if defined(NRF52840_FEATHER)
    #define BOARD_TYPE      "NRF52840_FEATHER"
  #elif defined(NRF52832_FEATHER)
    #define BOARD_TYPE      "NRF52832_FEATHER"
  #elif defined(NRF52840_FEATHER_SENSE)
    #define BOARD_TYPE      "NRF52840_FEATHER_SENSE"
  #elif defined(NRF52840_ITSYBITSY)
    #define BOARD_TYPE      "NRF52840_ITSYBITSY"
  #elif defined(NRF52840_CIRCUITPLAY)
    #define BOARD_TYPE      "NRF52840_CIRCUITPLAY"
  #elif defined(NRF52840_CLUE)
    #define BOARD_TYPE      "NRF52840_CLUE"
  #elif defined(NRF52840_METRO)
    #define BOARD_TYPE      "NRF52840_METRO"
  #elif defined(NRF52840_PCA10056)
    #define BOARD_TYPE      "NRF52840_PCA10056"
  #elif defined(NINA_B302_ublox)
    #define BOARD_TYPE      "NINA_B302_ublox"
  #elif defined(NINA_B112_ublox)
    #define BOARD_TYPE      "NINA_B112_ublox"
  #elif defined(PARTICLE_XENON)
    #define BOARD_TYPE      "PARTICLE_XENON"
  #elif defined(ARDUINO_NRF52_ADAFRUIT)
    #define BOARD_TYPE      "ARDUINO_NRF52_ADAFRUIT"
  #else
    #define BOARD_TYPE      "nRF52 Unknown"
  #endif
#endif

#ifndef BOARD_NAME
  #define BOARD_NAME    BOARD_TYPE
#endif

// To use faster 25MHz clock instead of defaulf 14MHz. Only for W5200 and W5500. W5100 also tested OK.
//#define USE_W5100     false

#define USE_ETHERNET_WRAPPER    true
//#define USE_ETHERNET_WRAPPER    false

// Use true  for ENC28J60 and UIPEthernet library (https://github.com/UIPEthernet/UIPEthernet)
// Use false for W5x00 and Ethernetx library      (https://www.arduino.cc/en/Reference/Ethernet)

//#define USE_UIP_ETHERNET   true
//#define USE_UIP_ETHERNET   false

//#define USE_CUSTOM_ETHERNET     true

// Note: To rename ESP628266 Ethernet lib files to Ethernet_ESP8266.h and Ethernet_ESP8266.cpp
// In order to USE_ETHERNET_ESP8266
#if ( !defined(USE_UIP_ETHERNET) || !USE_UIP_ETHERNET )

// To override the default CS/SS pin. Don't use unless you know exactly which pin to use
//#define USE_THIS_SS_PIN   22  //21  //5 //4 //2 //15

// Only one if the following to be true
#define USE_ETHERNET2         false //true
#define USE_ETHERNET3         false //true
#define USE_ETHERNET_LARGE    false //true
#define USE_ETHERNET_ESP8266  false //true

#if !USE_ETHERNET_WRAPPER

#if ( USE_ETHERNET2 || USE_ETHERNET3 || USE_ETHERNET_LARGE || USE_ETHERNET_ESP8266 )
  #ifdef USE_CUSTOM_ETHERNET
    #undef USE_CUSTOM_ETHERNET    
  #endif
  #define USE_CUSTOM_ETHERNET   true
#endif

#if USE_ETHERNET3
#include "Ethernet3.h"
#warning Use Ethernet3 lib
#elif USE_ETHERNET2
#include "Ethernet2.h"
#warning Use Ethernet2 lib
#elif USE_ETHERNET_LARGE
#include "EthernetLarge.h"
#warning Use EthernetLarge lib
#elif USE_ETHERNET_ESP8266
#include "Ethernet_ESP8266.h"
#warning Use Ethernet_ESP8266 lib
#elif USE_CUSTOM_ETHERNET
#include "Ethernet_XYZ.h"
#warning Use Custom Ethernet library from EthernetWrapper. You must include a library here or error.
#else
#define USE_ETHERNET          true
#include "Ethernet.h"
#warning Use Ethernet lib
#endif

// Ethernet_Shield_W5200, EtherCard, EtherSia not supported
// Select just 1 of the following #include if uncomment #define USE_CUSTOM_ETHERNET
// Otherwise, standard Ethernet library will be used for W5x00

#endif    //#if !USE_UIP_ETHERNET
#endif    //USE_ETHERNET_WRAPPER

#define USE_BLYNK_WM      true
//#define USE_BLYNK_WM    false

#if USE_BLYNK_WM

  #if USE_UIP_ETHERNET
    #include <BlynkSimpleUIPEthernet_WM.h>
  #else
    #include <BlynkSimpleEthernet_WM.h>
  #endif

  #define TO_LOAD_DEFAULT_CONFIG_DATA      false
  bool LOAD_DEFAULT_CONFIG_DATA = false;

  Blynk_Configuration defaultConfig;

  MenuItem myMenuItems [] = {};
  
  uint16_t NUM_MENU_ITEMS = 0;
  
#else

  #if USE_UIP_ETHERNET
    #include <BlynkSimpleUIPEthernet.h>
  #else
    #include <BlynkSimpleEthernet.h>
  #endif
  
  #define USE_LOCAL_SERVER      false //true
  
  // You should get Auth Token in the Blynk App.
  // Go to the Project Settings (nut icon).
  #if USE_LOCAL_SERVER
  
    // Local Blynk Server
    char token[] = "****";
    
    // Fill the name of Blynk Server
    char server[] = "account.ddns.net";
    //char server[] = "****.ddns.net";
    //char server[] = "192.168.2.112";
  
  #else
  
    //Cloud Blynk Server
    char token[] = "******";
    char server[] = "blynk-cloud.com";
  
  #endif

  #define BLYNK_HARDWARE_PORT     8080
  
#endif

WidgetLED led1(V1);

WidgetLCD lcd(V0);

unsigned short Temp_D1, Temp_D2;

BlynkTimer timer;

// V1 LED Widget is blinking
void blinkLedWidget()
{
  if (led1.getValue())
  {
    led1.off();
    Serial.println("LED on V1: off");
  }
  else
  {
    led1.on();
    Serial.println("LED on V1: on");
  }
}

int cnt = 0;

char Temp[32];

void doSomething()
{
  Serial.println("============ Print Every 120s ============");
  blinkLedWidget();
  Serial.println("========== And Blink LED widget ==========");
  sprintf(Temp, "%0d.%0d", Temp_D1, Temp_D2);
  lcd.print(0, 0, String(Temp));
}

void heartBeatPrint()
{
  static int num = 1;

  if (Blynk.connected())
    Serial.print(F("B"));     // For Blynk OK
  else
    Serial.print(F("F"));     // Failure

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

//#define BLYNK_CONNECT_TIMEOUT_MS      20000
#define USE_NON_BLOCKING_BLYNK        false


void setup()
{
  // Debug console
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("\nStart nRF52840_BLE_Scanner on " + String(BOARD_NAME));
  Serial.println(BLYNK_ETHERNET_WM_VERSION);

#if USE_BLYNK_WM
  #if USE_ETHERNET_WRAPPER

    EthernetInit();

  #else

    #if USE_ETHERNET
      LOGWARN(F("=========== USE_ETHERNET ==========="));
    #elif USE_ETHERNET2
      LOGWARN(F("=========== USE_ETHERNET2 ==========="));
    #elif USE_ETHERNET3
      LOGWARN(F("=========== USE_ETHERNET3 ==========="));
    #elif USE_ETHERNET_LARGE
      LOGWARN(F("=========== USE_ETHERNET_LARGE ==========="));
    #elif USE_ETHERNET_ESP8266
      LOGWARN(F("=========== USE_ETHERNET_ESP8266 ==========="));
    #else
      LOGWARN(F("========================="));
    #endif
   
      LOGWARN(F("Default SPI pinout:"));
      LOGWARN1(F("MOSI:"), MOSI);
      LOGWARN1(F("MISO:"), MISO);
      LOGWARN1(F("SCK:"),  SCK);
      LOGWARN1(F("SS:"),   SS);
      LOGWARN(F("========================="));
       
    #if defined(ESP8266)
      // For ESP8266, change for other boards if necessary
      #ifndef USE_THIS_SS_PIN
        #define USE_THIS_SS_PIN   D2    // For ESP8266
      #endif
      
      LOGWARN1(F("ESP8266 setCsPin:"), USE_THIS_SS_PIN);
      
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
      
      LOGWARN1(F("ESP32 setCsPin:"), USE_THIS_SS_PIN);
      
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
           
      LOGWARN1(F("Unknown board setCsPin:"), USE_THIS_SS_PIN);
  
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



  Blynk.begin();
  
#else

  #if USE_LOCAL_SERVER
    #if USE_NON_BLOCKING_BLYNK
      Blynk.config(token, server, BLYNK_HARDWARE_PORT);
      Blynk.connect(BLYNK_CONNECT_TIMEOUT_MS);
    #else
      //Blynk.begin(token);
      // You can also specify server:
      Blynk.begin(token, server, BLYNK_HARDWARE_PORT);
    #endif
  #else
    Blynk.begin(token);
    // You can also specify server:
    //Blynk.begin(token, server, BLYNK_HARDWARE_PORT);
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
    Serial.println(BLYNK_HARDWARE_PORT);
    Serial.print(F("Token = "));
    Serial.print(token); 
#endif
    Serial.print(F(", IP = "));   
    Serial.println(Ethernet.localIP());
  }

  // Every 120s, doSomething => print and blink LED (V1) on APP
  timer.setInterval(5000L, doSomething);

  Serial.println("Bluefruit52 Central Scan Example");
  Serial.println("--------------------------------\n");

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // SRAM usage required by SoftDevice will increase dramatically with number of connections
  Bluefruit.begin(0, 1);
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName("Bluefruit52");

  // Start Central Scan
  Bluefruit.setConnLedInterval(250);
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.start(0);

  Serial.println("Scanning ...");
}

//DB:9C:ED:D2:A2:F7
unsigned char smart_watch_mac[] = {0xDB, 0x9C, 0xED, 0xD2, 0xA2, 0xF7};

void scan_callback(ble_gap_evt_adv_report_t* report)
{
  if (report->peer_addr.addr[5] == smart_watch_mac[0])
    if (report->peer_addr.addr[4] == smart_watch_mac[1])
      if (report->peer_addr.addr[3] == smart_watch_mac[2])
        if (report->peer_addr.addr[2] == smart_watch_mac[3])
          if (report->peer_addr.addr[1] == smart_watch_mac[4])
            if (report->peer_addr.addr[0] == smart_watch_mac[5])
            {
              // MAC is in little endian --> print reverse
              Serial.printBufferReverse(report->peer_addr.addr, 6, ':');
              Serial.printBuffer(report->data.p_data, report->data.len, '-');
              Serial.println();
              Serial.print(report->data.p_data[11]);
              Serial.print(".");
              Serial.println(report->data.p_data[12]);
              Temp_D1 = report->data.p_data[11]; //to blynk
              Temp_D2 = report->data.p_data[12]; //to blynk
            }
  // For Softdevice v6: after received a report, scanner will be paused
  // We need to call Scanner resume() to continue scanning
  Bluefruit.Scanner.resume();
}

void loop()
{
  Blynk.run();
  timer.run();
  check_status();
}
