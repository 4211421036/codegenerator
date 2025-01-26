
/**************************************************************
  项目说明：超级图表
  APP设置：
  创建Super Chart组件，
  创建一个datastreams，管脚设为V5；
  创建一个datastreams，管脚设为V6；
  
  *************************************************************/
     #define BLYNK_PRINT Serial // 开启串口监视
     #include <ESP8266WiFi.h>
     #include <BlynkSimpleEsp8266.h>
     #include <DHT.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

void myTimerEvent()
{
  // 发送数据的频率不要超过每秒10次。
  Blynk.virtualWrite(V5, millis() / 1000);
  Blynk.virtualWrite(V6, millis() / 1000);
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  // Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 163), 8080);
   timer.setInterval(1000L, myTimerEvent);//每隔1000毫秒执行一次myTimerEvent函数

  
}

void loop()
{
  Blynk.run();
  timer.run(); //初始化定时器
}

