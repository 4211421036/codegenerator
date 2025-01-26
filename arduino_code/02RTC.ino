/*************************************************************
  项目说明：获取时间/日期
  App项目设置:创建RTC组件，
  温馨提示：
  本项目需要额外库
  https://github.com/PaulStoffregen/Time
  本项目基于
  https://github.com/PaulStoffregen/Time/blob/master/examples/TimeSerial/TimeSerial.ino
  *************************************************************/
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码
BlynkTimer timer;
WidgetRTC rtc;//创建rtc组件
void clockDisplay()//显示时间
{
  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(year()) + "-" + month() + "-" + day();
  Serial.print("Current time: ");
  Serial.println(currentTime);
  Serial.print("Current Date: ");
  Serial.println(currentDate);
  Serial.print("Current Weekday: ");
  Serial.println(weekday());//显示星期值1-7，对应周日-周六
}

BLYNK_CONNECTED() {
  rtc.begin();//连上后同步时间
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  // 更多资料，请参考: http://www.pjrc.com/teensy/td_libs_Time.html
  setSyncInterval(10 * 60); // 设置同步间隔时间，10分钟。
  timer.setInterval(1000L, clockDisplay);//每隔1s，运行clockDisplay，显示时间
}

void loop()
{
  Blynk.run();
  timer.run();
}

