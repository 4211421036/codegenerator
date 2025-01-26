/*************************************************************
  项目说明：串口输出摇杆值

  App项目设置:
  创建Joystick组件，输出管脚设置为V1，模式设置为MERGE

  Joystick组件设置说明：
  - 设置Joystick名称
  - OUTPUT:SPLIT(分离模式);MERGE(合并模式)。
  选择分离模式时，将使用两个管脚来传输摇杆的x/y数值；
  选择合并模式时，将使用一个虚拟管脚来传输摇杆的x/y数值；
  - AUTORETURN:摇杆自动回位
  - ROTATE ON TILE:手机横屏时自动调整，开启该功能时，方便手机横过来操作
  - WRITE INTERVAL:写入时间间隔，为防止大量数据写入而设置的写入时间间隔

  blynk_for_chinese 库地址：https://github.com/hznupeter/blynk-library-for-chinese

  Blynk物联网学习资料：https://gitee.com/hznupeter/Blynk_IOT/wikis
 *************************************************************/
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

BLYNK_WRITE(V1) {//设置joystick组件管脚为V1,模式为merge
  int x = param[0].asInt();//joystick的x值
  int y = param[1].asInt();//joystick的y值
  Serial.print("X = ");
  Serial.print(x);
  Serial.print("; Y = ");
  Serial.println(y);
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  // Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
}

void loop()
{
  Blynk.run();
}
