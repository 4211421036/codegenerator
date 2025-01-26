/*************************************************************
  项目说明：RTC高级应用
  App项目设置:创建RTC组件
  温馨提示：
  Blynk可以给设备授时，能起到类似于RTC的作用，
  但是，该方法的精度最高是几秒钟。
 *************************************************************/
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

BlynkTimer timer;

void requestTime() {//查询时间
  Blynk.sendInternal("rtc", "sync");
}

BLYNK_WRITE(InternalPinRTC) {
  long t = param.asLong();
  Serial.print("Unix time: ");
  Serial.print(t);
  Serial.println();
}

void setup()
{

  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  timer.setInterval(10000L, requestTime);//每隔10s运行requestTime
}

void loop()
{
  Blynk.run();
  timer.run();
}

