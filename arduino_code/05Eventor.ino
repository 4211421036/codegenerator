/*************************************************************
  项目说明：发送邮件
  App项目设置:创建eventor组件
  硬件设置：
  注意事项：

  You can use predefined rules on application side.
  Project setup in the Blynk app:
    Eventor widget with next rules:
      a) When V0 is equal to 1, set V1 to 255;
      b) When V0 is equal to 0, set V1 to 0;
    Led widget on V1 pin
 *************************************************************/
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

BlynkTimer timer;
boolean flag = true;
void sendFlagToServer() {
  if (flag) {
    Blynk.virtualWrite(V0, 1);
  } else {
    Blynk.virtualWrite(V0, 0);
  }
  flag = !flag;
}

BLYNK_WRITE(V1) {
  int ledValue = param.asInt();
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  timer.setInterval(1000L, sendFlagToServer);//每秒执行一次
}

void loop()
{
  Blynk.run();
  timer.run();
}

