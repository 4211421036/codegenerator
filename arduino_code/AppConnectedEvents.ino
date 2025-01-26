/*************************************************************
  项目说明：APP连接信息
  App项目设置:开启"notify devices when app connected"
 *************************************************************/
#define BLYNK_PRINT Serial // 开启串口监视
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "c3102448027d494ca48b27d0a732f245";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "password";//wifi密码

// 当app开启时，会调用该函数
BLYNK_APP_CONNECTED() {
  Serial.println("App Connected.");
}

//// 当app关闭时，会调用该函数
BLYNK_APP_DISCONNECTED() {
  Serial.println("App Disconnected.");
}


void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 31, 140), 8080);//自建服务器ip模式
}

void loop()
{
  Blynk.run();
}

