/*************************************************************
  项目说明：LCD高级模式
  App项目设置:创建LCD组件，设置为ADVANCED模式，
  输入管脚为V1
 *************************************************************/
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

WidgetLCD lcd(V1);

void setup()
{
  Serial.begin(9600);
  // Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式

  lcd.clear(); //LCD清屏
  lcd.print(4, 0, "Hello"); // 使用格式: (X坐标: 0-15, Y坐标: 0-1, "Message you want to print")
  lcd.print(4, 1, "World");
  //如果在主函数中使用该函数，请通过调用事件的方式显示，如果直接显示会导致硬件掉线。
}

void loop()
{
  Blynk.run();
}

