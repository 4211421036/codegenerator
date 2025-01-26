/****************************************************************************************************************************
  SAM_DUE_ESP8266Shield.ino
  For SAM DUE boards using ESP8266 AT WiFi Shields, using much less code to support boards with smaller memory

  ESP_AT_WM_Lite is a library for the Mega, Teensy, SAM DUE, SAMD and STM32 boards (https://github.com/khoih-prog/ESP_AT_WM_Lite)
  to enable store Credentials in EEPROM to easy configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services
  without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/ESP_AT_WM_Lite
  Licensed under MIT license
 *****************************************************************************************************************************/

#include "defines.h"
#include "Credentials.h"
#include "dynamicParams.h"

ESP_AT_WiFiManager_Lite* ESP_AT_WiFiManager;

void heartBeatPrint()
{
  static int num = 1;

  if (ESP_AT_WiFiManager->getWiFiStatus())
    Serial.print("H");        // H means connected to WiFi
  else
  {
    if (ESP_AT_WiFiManager->isConfigMode())
      Serial.print("C");        // C means in Config Mode
    else
      Serial.print("F");        // F means not connected to WiFi
  }

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(" ");
  }
}

void check_status()
{
  static unsigned long checkstatus_timeout = 0;

  //KH
#define HEARTBEAT_INTERVAL    20000L

  // Print hearbeat every HEARTBEAT_INTERVAL (20) seconds.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    heartBeatPrint();
    checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
  }
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  while (!Serial);

  Serial.print("\nStart SAM_DUE_ESP8266Shield on ");
  Serial.println(BOARD_NAME);
  Serial.println(ESP_AT_WM_LITE_VERSION);
  Serial.print("Debug Level = ");
  Serial.println(_ESP_AT_WM_LOGLEVEL_);

  // initialize serial for ESP module
  EspSerial.begin(115200);

  ESP_AT_WiFiManager = new ESP_AT_WiFiManager_Lite(&EspSerial, ESP8266_BAUD);

  // Optional to change default AP IP(192.168.4.1) and channel(10)
  //ESP_AT_WiFiManager->setConfigPortalIP(IPAddress(192, 168, 220, 1));
  ESP_AT_WiFiManager->setConfigPortalChannel(2);

  // Personalized portal_ssid and password
  ESP_AT_WiFiManager->setConfigPortal(portal_ssid, portal_password);

  ESP_AT_WiFiManager->begin();
}

#if USE_DYNAMIC_PARAMETERS
void displayCredentials()
{
  Serial.println("\nStored Dynamic Params:");

  for (uint8_t i = 0; i < NUM_MENU_ITEMS; i++)
  {
    Serial.print(myMenuItems[i].displayName);
    Serial.print(" = ");
    Serial.println(myMenuItems[i].pdata);
  }
}

void displayCredentialsOnce()
{
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
}
#endif

void loop()
{
  ESP_AT_WiFiManager->run();
  check_status();

#if (USE_DYNAMIC_PARAMETERS)
  displayCredentialsOnce();
#endif
}
