/*************************************************************
  项目说明：音乐播放器
  App项目设置:创建player组件，设置管脚为V5
  *************************************************************/
  #define BLYNK_PRINT Serial

  #include <ESP8266WiFi.h>
  #include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

BLYNK_WRITE(V5)
{
  String action = param.asStr();

  if (action == "play") {
    // Do something
  }
  if (action == "stop") {
    // Do something
  }
  if (action == "next") {
    // Do something
  }
  if (action == "prev") {
    // Do something
  }

  Blynk.setProperty(V5, "label", action);
  Serial.print(action);
  Serial.println();
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

