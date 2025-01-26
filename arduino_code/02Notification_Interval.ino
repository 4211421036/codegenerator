/*************************************************************
  项目说明：定时器发送通知到手机

  APP端组件设置说明：
  - 添加notification组件
 *************************************************************/
#define BLYNK_PRINT Serial // 开启串口监视
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码
BlynkTimer timer;

void notifyUptime()
{
  long uptime = millis() / 60000L;
  Blynk.notify(String("Running for ") + uptime + " minutes.");//通知手机该项目运行时间
  // 你也可以使用 {DEVICE_NAME}来代替项目名称，用法如下：
  // Blynk.notify(String("{DEVICE_NAME} running for ") + uptime + " minutes.");
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  Blynk.notify("Device started");//开机是发送通知
  timer.setInterval(60000L, notifyUptime);//每分钟调用一次该函数
}

void loop()
{
  Blynk.run();
  timer.run();
}