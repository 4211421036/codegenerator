/*************************************************************
  项目说明：app端LED闪烁

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

WidgetLED led1(V1);//在V1创建一个LED组件

BlynkTimer timer;

void blinkLedWidget()// app端LED闪烁函数
{
  if (led1.getValue()) {//如果LED组件是点亮的，就关闭
    led1.off();
    Serial.println("LED on V1: off");
  } else {
    led1.on();
    Serial.println("LED on V1: on");
  }
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  timer.setInterval(1000L, blinkLedWidget);//每隔1秒执行blinkLedWidget函数
}

void loop()
{
  Blynk.run();
  timer.run();
}
