/*************************************************************
  项目说明：定时器发送通知到手机

  APP端组件设置说明：
  - 添加notification组件

  硬件连接：
  GPIO2：连接按钮传感器
 *************************************************************/
#define BLYNK_PRINT Serial // 开启串口监视
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码
BlynkTimer timer;

void notifyOnButtonPress()
{
  int isButtonPressed = !digitalRead(2);
  if (isButtonPressed) {
    Serial.println("Button is pressed.");
    Blynk.notify("Yaaay... button is pressed!");
    // 你也可以使用 {DEVICE_NAME}来代替项目名称，用法如下：
    // Blynk.notify(String("{DEVICE_NAME} running for ") + uptime + " minutes.");
  }
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  pinMode(2, INPUT_PULLUP);//设置GPIO2为中断管脚
  attachInterrupt(digitalPinToInterrupt(2), notifyOnButtonPress, CHANGE);//GPIO2添加中断函数
}

void loop()
{
  Blynk.run();
}
