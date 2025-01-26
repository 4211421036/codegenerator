/****************************************************************************************************************************
  W5500_Blynk_Email_SAMD.ino
  For Teensy, SAM DUE, SAMD boards using W5100 Ethernet shields
  
  BlynkEthernet_WM is a library for Teensy, ESP, SAM DUE and SAMD boards, with Ethernet W5X00 or ENC28J69 shields,
  to enable easy configuration/reconfiguration and autoconnect/autoreconnect of Ethernet/Blynk
  
  Library forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
  Built by Khoi Hoang https://github.com/khoih-prog/Blynk_WM
  Licensed under MIT license
 *****************************************************************************************************************************/
#include "defines.h"
#include "Credentials.h"
#include "dynamicParams.h"

#include <SPI.h>

BlynkTimer timer;

#define BUTTON_PIN      2

volatile unsigned int count       = 0;
volatile bool isButtonPressed     = false;


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

void emailOnButtonPress()
{
  //isButtonPressed = !digitalRead(BUTTON_PIN); // Invert state, since button is "Active LOW"

  if ( !isButtonPressed && !digitalRead(BUTTON_PIN)) // You can write any condition to trigger e-mail sending
  {
    isButtonPressed = true;
    count++;
    Serial.println("Button pressed");
  }
}

void processButton()
{
  // *** WARNING: You are limited to send ONLY ONE E-MAIL PER 5 SECONDS! ***
  // Let's send an e-mail when you press the button
  // connected to digital pin BUTTON_PIN (2) on your Arduino
  static String body;

  if (isButtonPressed) // You can write any condition to trigger e-mail sending
  {
    body = String("You pushed the button ") + count + " times.";

    // This can be seen in the Serial Monitor
    Serial.println(body);

    Blynk.email("your_email@gmail.com", "Subject: Button Logger", body);

    isButtonPressed = false;
  }

}

void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStart W5500_Blynk_Email_SAMD on ")); Serial.print(BOARD_NAME);
  Serial.print(F(" with ")); Serial.println(SHIELD_TYPE);

#if USE_BLYNK_WM 
  Serial.println(BLYNK_ETHERNET_WM_VERSION);
#endif

  pinMode(BUTTON_PIN, INPUT_PULLUP);

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
  
  #endif  //USE_ETHERNET_WRAPPER

#if USE_BLYNK_WM
  Blynk.begin();
#else
#if USE_LOCAL_SERVER
  Blynk.begin(auth, server, BLYNK_SERVER_HARDWARE_PORT);
#else
  Blynk.begin(auth);
  // You can also specify server:
  //Blynk.begin(auth, server, BLYNK_SERVER_HARDWARE_PORT);
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
    Serial.print(F(", IP = "));   
#else
    Serial.print(F("Conn2Blynk: server = "));
    Serial.print(server);
    Serial.print(F(", port = "));
    Serial.println(BLYNK_SERVER_HARDWARE_PORT);
    Serial.print(F("Token = "));
    Serial.print(auth);
    Serial.print(F(", IP = "));       
#endif    
    Serial.println(Ethernet.localIP());
  }

  // Attach pin BUTTON_PIN (2) interrupt to our handler
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), emailOnButtonPress, FALLING /*CHANGE*/);

  timer.setInterval(30000L, processButton);
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
  timer.run();
  check_status();

#if (USE_BLYNK_WM && USE_DYNAMIC_PARAMETERS)
  displayCredentialsInLoop();
#endif 
}
