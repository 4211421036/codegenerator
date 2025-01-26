/*!
 *@file       : Basic Blynk Counter.ino
 *@brief      : Blynk display counter 0 to 100.
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

//Global Counter Index
short gsCounter_Index = 0;

void setup() 
{
  // Debug console
  Serial.begin(9600);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  //Connect to Blynk
  DBlynk.begin(tempID, deviceName, auth, ssid, pass);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  DBlynk.run();
  
  //Set counter index to virtual pin V0 
  DBlynk.virtualWrite(V0, gsCounter_Index);

  gsCounter_Index++;
  if(gsCounter_Index > 100) //if counter index more than 100, reset to 0
  {
    gsCounter_Index = 0;  
  }

  delay(1000);
}
