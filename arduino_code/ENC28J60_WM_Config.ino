/****************************************************************************************************************************
  ENC28J60_WM_Config.ino
  For Mega, Teensy, SAM DUE, SAMD boards using W5100 Ethernet shields
  
  BlynkEthernet_WM is a library for Mega AVR, Teensy, ESP, SAM DUE and SAMD boards, with Ethernet W5X00 or ENC28J69 shields,
  to enable easy configuration/reconfiguration and autoconnect/autoreconnect of Ethernet/Blynk
  
  Library forked from Blynk library v0.6.1 https://github.com/blynkkk/blynk-library/releases
  Built by Khoi Hoang https://github.com/khoih-prog/Blynk_WM
  Licensed under MIT license   
 *****************************************************************************************************************************/

#include <SPI.h>

#include "defines.h"
#include "Credentials.h"

BlynkTimer timer;

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

void readAndSendData()
{
  float temperature = 25.0;
  float humidity    = 50.0;

  if (Blynk.connected())
  {
    if (!isnan(temperature) && !isnan(humidity))
    {
      Blynk.virtualWrite(V17, String(temperature, 1));
      Blynk.virtualWrite(V18, String(humidity, 1));
    }
    else
    {
      Blynk.virtualWrite(V17, F("NAN"));
      Blynk.virtualWrite(V18, F("NAN"));
    }
  }

  // Blynk Timer uses millis() and is still working even if WiFi/Blynk not connected
  Serial.print(F("R"));
}

void heartBeatPrint()
{
  static int num = 1;

  if (Blynk.connected())
    Serial.print(F("B"));
  else
    Serial.print(F("F"));

  if (num == 40)
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

void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial);

  delay(200);

#if USE_BLYNK_WM

#if ( USE_LITTLEFS || USE_SPIFFS)
  Serial.print(F("\nStart ENC28J60_WM_Config using ")); Serial.print(CurrentFileFS);
  Serial.print(F(" on ")); Serial.print(BOARD_NAME);
#else
  Serial.print(F("\nStart ENC28J60_WM_Config on ")); Serial.print(BOARD_NAME);
#endif

  Serial.print(F(" with ")); Serial.println(SHIELD_TYPE);
  Serial.println(BLYNK_ETHERNET_WM_VERSION);

  Blynk.begin();
#else
#if USE_LOCAL_SERVER
  Serial.println("\nStart ENC28J60_WM_Config, no WM, on " + String(BOARD_TYPE) + String(" using Local Server"));
  Blynk.begin(auth, server, BLYNK_HARDWARE_PORT);
#else
  Serial.println("\nStart ENC28J60_WM_Config, no WM, on " + String(BOARD_TYPE) + String(" using Cloud Server"));
  //Blynk.begin(auth);
  // You can also specify server:
  Blynk.begin(auth, server, BLYNK_HARDWARE_PORT);
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
    Serial.println(BLYNK_HARDWARE_PORT);
    Serial.print(F("Token = "));
    Serial.print(auth);
    Serial.print(F(", IP = "));       
#endif    
    Serial.println(Ethernet.localIP());
  }

  timer.setInterval(60000L, readAndSendData);
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
