/*************************************************************
  项目说明：控制app端LED亮度

  APP端组件设置说明：
  - 设置LED名称,设置LED颜色
  - INPUT管脚:设置虚拟管脚
*************************************************************/
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

WidgetLED led2(V2);//在V2创建一个LED组件

BlynkTimer timer;

void fadeLedWidget()//led亮度变化函数
{
  static int value = 0;
  static int delta = 30;
  value += delta;
  if (value > 255 || value < 0) {
    delta = -delta;
  } else {
    Serial.print("LED on V2: ");
    Serial.println(value);
    led2.setValue(value);
  }
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  timer.setInterval(300L, fadeLedWidget);//每隔300毫秒执行fadeLedWidget函数
}

void loop()
{
  Blynk.run();
  timer.run();
}

