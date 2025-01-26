/*************************************************************
  项目说明：table组件初级用法
  App项目设置:
  创建table组件，输入管脚设置为V1
 *************************************************************/
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

BlynkTimer timer;
int rowIndex = 0;

void sendEvent() {
  // adding 1 row to table every second
  Blynk.virtualWrite(V1, "add", rowIndex, "My Event", millis() / 1000);
  //增加行，增加内容为millis() / 1000
  //highlighting latest added row in table
  Blynk.virtualWrite(V1, "pick", rowIndex);//标注行

  //其他用法
  //Blynk.virtualWrite(V1, "add", id, "Name", "Value");//增加行
  // Blynk.virtualWrite(V1, "update", id, "UpdatedName", "UpdatedValue");//更新行
  //Blynk.virtualWrite(V1, "select", 0);//选择第0行
  //Blynk.virtualWrite(V1, "deselect", 0);//取消选择第0行
  rowIndex++;
}

void setup()
{
  Serial.begin(9600);
  // Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  Blynk.virtualWrite(V1, "clr");//开始运行时先清空table
  timer.setInterval(1000L, sendEvent);//每隔1000毫秒执行sendEvent
}

void loop()
{
  Blynk.run();
  timer.run();
}

