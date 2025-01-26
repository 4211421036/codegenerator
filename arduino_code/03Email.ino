/*************************************************************
  项目说明：发送邮件
  App项目设置:创建e-mail组件
  硬件连接：按钮传感器连接在GPIO2管脚
  注意事项：最高频率5秒1封邮件

 *************************************************************/

#define BLYNK_PRINT Serial

#define BLYNK_MAX_SENDBYTES 128// 该值越大，能发送的邮件长度越长

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

void emailOnButtonPress()
{
  int isButtonPressed = !digitalRead(2); 
  if (isButtonPressed) // 你可以在此设置任何触发邮件发送的条件
  {//按钮按下时，发送邮件
    Serial.println("Button is pressed."); // 串口输出
    Blynk.email("your_email@mail.com", "Subject: Button Logger", "You just pushed the button...");
    // 如果只是想在手机app上显示，可以用以下格式
    //Blynk.email("Subject: Button Logger", "You just pushed the button...");
  }
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  //当开发板连接到服务器时，发送一封邮件
  //收件人，邮件主题，邮件内容
  Blynk.email("your_email@mail.com", "Subject", "My Blynk project is online.");
  // 设置按钮管脚为中断输入
  pinMode(2, INPUT_PULLUP);
  // 为GPIO2设置中断，中断函数为emailOnButtonPress
  attachInterrupt(digitalPinToInterrupt(2), emailOnButtonPress, CHANGE);
}

void loop()
{
  Blynk.run();
}
