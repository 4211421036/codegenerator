/*************************************************************
  项目说明：控制IO电平
  App项目设置:
  创建Button组件，输出管脚设置为GPIO2,模式设为Switch

  硬件连接：
  闪灯模块连接在GPIO2管脚
  
  blynk_for_chinese 库地址：https://github.com/hznupeter/blynk-library-for-chinese

  Blynk物联网学习资料：https://gitee.com/hznupeter/Blynk_IOT/wikis
  *************************************************************/
  #define BLYNK_PRINT Serial // 开启串口监视
  #include <ESP8266WiFi.h>
  #include <BlynkSimpleEsp8266.h>
char auth[] = "c3102448027d494ca48b27d0a732f245";//授权码
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
