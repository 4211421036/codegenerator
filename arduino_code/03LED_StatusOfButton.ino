/*************************************************************
  项目说明：按钮状态指示灯
  
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

const int btnPin = 12;// 按钮连接管脚

WidgetLED led3(V3);//在V3创建一个LED组件

BlynkTimer timer;

boolean btnState = false;// 用LED表示按钮是否按下

void buttonLedWidget()
{
  boolean isPressed = (digitalRead(btnPin) == LOW);//读取按钮是否按下，注意，此处是针对 低电平 有效的按钮
  if (isPressed != btnState) {//如果当前按钮状态与之前不一样
    if (isPressed) {//如果当前是按下的
      led3.on();//点亮led
    } else {
      led3.off();//关闭led
    }
    btnState = isPressed;
  }
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  pinMode(btnPin, INPUT_PULLUP);//btnPin管脚设为上升中断模式
  timer.setInterval(500L, buttonLedWidget);//每隔500毫秒执行buttonLedWidget
}

void loop()
{
  Blynk.run();
  timer.run();
}
