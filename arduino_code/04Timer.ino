/*************************************************************
  项目说明：定时控制LED亮灭

  App项目设置:
  创建Timer组件，输出管脚设置为GPIO2，设置开始时间和结束时间

  Timer组件设置说明：
  - 设置Timer名称,如定时开灯
  - OUTPUT管脚:可以设置为数字管脚,模拟管脚或者虚拟管脚，如设置为GPIO2
  - START/STOP TIMERS:设置开始/停止时间
  - DESIGN:FONT SIZE：字体大小；TEXT：文字颜色

   硬件连接：
  LED连接到GPIO2

  blynk_for_chinese 库地址：https://github.com/hznupeter/blynk-library-for-chinese

  Blynk物联网学习资料：https://gitee.com/hznupeter/Blynk_IOT/wikis
*************************************************************/
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码
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

