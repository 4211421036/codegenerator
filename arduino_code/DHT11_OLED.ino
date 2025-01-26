/*************************************************************
  项目说明：读取DHT11并显示在OLED
  App项目设置:
  创建Value Display组件，输入管脚设置为V5,数值范围0-100,reading rate设置为push
  创建Value Display组件，输入管脚设置为V6,数值范围-20-60,reading rate设置为push
  
  硬件连接：
  OLED连接在IIC接口
  DHT11连接在GPIO2管脚

  blynk_for_chinese 库地址：https://github.com/hznupeter/blynk-library-for-chinese

  Blynk物联网学习资料：https://gitee.com/hznupeter/Blynk_IOT/wikis
  *************************************************************/
  #define BLYNK_PRINT Serial // 开启串口监视
  #include <ESP8266WiFi.h>
  #include <BlynkSimpleEsp8266.h>
  #include <DHT.h>
  #include <Arduino.h>
  #include <U8g2lib.h>
  #include <Wire.h>
  char auth[] = "14b1f8e400b64d53b1c557cad983a114";
  char ssid[] = "ssid";
  char pass[] = "pass";
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
#define DHTPIN 2//传感器连接管脚
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;
void setup(void) {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 31, 249), 8080);//自建服务器ip模式
  dht.begin();
  timer.setInterval(1000L, sendSensor);
  u8g2.begin();
}
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); //摄氏度
  //float t = dht.readTemperature(true); //华氏度
  Blynk.virtualWrite(V0, t);//将湿度发送给V0
  Blynk.virtualWrite(V1, h);//将湿度发送给V1
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr); 
  u8g2.setFontPosTop();
  u8g2.drawStr(0,0,"WENDU:");
  u8g2.setCursor(60, 0);
  u8g2.print(t);
  u8g2.drawStr(0,20,"SHIDU:");
  u8g2.setCursor(60, 20);
  u8g2.print(h);
  u8g2.sendBuffer();
}
void loop(void) {
  Blynk.run();
  timer.run();
}

