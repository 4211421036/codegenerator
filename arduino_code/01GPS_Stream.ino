/*************************************************************
  项目说明：GPS
  App项目设置:创建 GPS 组件，设置管脚为V1
  硬件设置：
  注意事项：需要google play服务支持

*************************************************************/

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

BLYNK_WRITE(V1) {
  GpsParam gps(param);

  // Print 6 decimal places for Lat, Lon
  Serial.print("Lat: ");
  Serial.println(gps.getLat(), 7);

  Serial.print("Lon: ");
  Serial.println(gps.getLon(), 7);

  // Print 2 decimal places for Alt, Speed
  Serial.print("Altitute: ");
  Serial.println(gps.getAltitude(), 2);

  Serial.print("Speed: ");
  Serial.println(gps.getSpeed(), 2);

  Serial.println();
}

void setup()
{
  Serial.begin(9600);
  // Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
}

void loop()
{
  Blynk.run();
}

