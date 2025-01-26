/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example shows how to use the Menu Widget.
  App project setup:
    Menu widget attached to V1 (put 3 items in it)
 *************************************************************/
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码


BLYNK_WRITE(V1) {
  switch (param.asInt())
  {
    case 1: // Item 1
      Serial.println("Item 1 selected");
      break;
    case 2: // Item 2
      Serial.println("Item 2 selected");
      break;
    case 3: // Item 3
      Serial.println("Item 3 selected");
      break;
    default:
      Serial.println("Unknown item selected");
  }
}

void setup()
{
    Serial.begin(9600);
   Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  
}

void loop()
{
  Blynk.run();
}

