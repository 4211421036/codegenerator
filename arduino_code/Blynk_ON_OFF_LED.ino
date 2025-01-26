/*!
 *@file       : Blynk ON OFF LED.ino
 *@brief      : On Off LEDs via Blynk Widget Button.
 *@copyright  : Copyright (c) 2022 MYBOTIC (http://www.mybotic.com.my)
 *@licence    : The MIT License (MIT)
 *@author     : Ng Kwang Chen
 *@version    : V1.0
 *@date       : 5-6-2022
 *@get from https://www.mybotic.com.my
 *@https://github.com/MYBOTIC/DurianBlynkESP8266
 *
 *Note: The library only work with Esp8266 Serial Module with Mybotic Durian ESP8266 Firmware.
*/

//BLYNK_TEMPLATE_ID & BLYNK_DEVICE_NAME can be commented if you wish to use old blynk
#define BLYNK_TEMPLATE_ID           "TMPLxxxxxx"
#define BLYNK_DEVICE_NAME           "Device"
#define BLYNK_AUTH_TOKEN            "YourAuthToken"

#include "DurianBlynkESP8266.h"

//uncomment this if using Serial for ESP
//#define EspSerial Serial

//comment this if using Serial for ESP
// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX

//ESP8266 baud rate default 9600
#define ESP8266_BAUD 9600

//Define Durian UNO Output pin
#define OUT1 7
#define OUT2 8
#define OUT3 4
#define OUT4 5

short gsOut_PinArray[4] = {OUT1, OUT2, OUT3, OUT4};

char auth[] = BLYNK_AUTH_TOKEN;

#ifdef BLYNK_TEMPLATE_ID && BLYNK_DEVICE_NAME
char tempID[] = BLYNK_TEMPLATE_ID; 
char deviceName[] = BLYNK_DEVICE_NAME; 
#else
char tempID[5]; 
char deviceName[5]; 
#endif

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

//esp8266 serial
DBlynk DBlynk(&EspSerial);

void setup() 
{
  // Debug console
  Serial.begin(9600);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  //Connect to Blynk
  DBlynk.begin(tempID, deviceName, auth, ssid, pass);

  //set Output pin to output
  for(short i=0; i<4; i++)
  {
    pinMode(gsOut_PinArray[i], OUTPUT);
    digitalWrite(gsOut_PinArray[i], LOW);
  }
}

void loop() 
{
  // put your main code here, to run repeatedly:
  DBlynk.run();

  //checking virtual pin V0 to V4, if high turn on the output pin. Otherwise, turn off
  for(short i=V0; i<V4; i++)
  {
    if(DBlynk.read_asInt(i) == 1)
    {
      digitalWrite(gsOut_PinArray[i], HIGH);
    }
    else
    {
      digitalWrite(gsOut_PinArray[i], LOW);
    }
  }

}
