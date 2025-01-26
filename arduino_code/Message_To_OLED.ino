/**************************************************************
项目说明：OLED显示
手机端发送的文字显示在OLED屏幕上
硬件连接：OLED 连接在IIC接口
温馨提示：该项目需要下载以下第三方库
https://github.com/olikraus/u8g2
更多示例，请参考https://github.com/olikraus/u8g2/wiki/u8g2reference
app端设置：
创建Terminal组件，输入管脚V1，开启INPUT LINE

blynk_for_chinese 库地址：https://github.com/hznupeter/blynk-library-for-chinese

  Blynk物联网学习资料：https://gitee.com/hznupeter/Blynk_IOT/wikis
*************************************************************/
#define BLYNK_PRINT Serial // 开启串口监视
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
WidgetTerminal terminal(V1);
void setup()
{
  Serial.begin(9600);
  u8g2.begin();
  u8g2.enableUTF8Print();
  Blynk.begin(auth, ssid, pass);
  //也可以用以下方式连接到服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 163), 8080);
}
BLYNK_WRITE(V1)
{
  u8g2.setFont(u8g2_font_t0_16b_tr);  // 设置字体
  //字体列表https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.setFontDirection(0);//设置文字方向
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);//设置文字显示位置
  u8g2.print(param.asStr());//将接收到的文字显示出来
  u8g2.sendBuffer();
  terminal.flush();
}
void loop()
{
  Blynk.run();
}

