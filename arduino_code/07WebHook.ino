/*************************************************************
  项目说明：使用webHook获取数据
  App项目设置:创建WebHook组件，设置V0管脚
  method: GET, url: /pin/
*************************************************************/
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

BLYNK_WRITE(V0)
{
  Serial.println("WebHook data:");
  Serial.println(param.asStr());
}

void setup()
{
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  Blynk.virtualWrite(V0, "https://raw.githubusercontent.com/blynkkk/blynk-library/master/extras/logo.txt");

  // You can perform HTTPS requests even if your hardware alone can't handle SSL
  // 如果您使用的开发板有足够的RAM，
  //Blynk也可以接收更大的消息。你可以设置BLYNK_MAX_READBYTES 为4096
  //Blynk.virtualWrite(V0, "https://api.sunrise-sunset.org/json?lat=50.4495484&lng=30.5253873&date=2016-10-01");
}

void loop()
{
  Blynk.run();
}

