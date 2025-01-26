/**************************************************************
  项目说明：用Step控制LED亮度

  App项目设置:
  创建STEP组件，输出管脚设置为GPIO2，step设置为100

   STEP组件设置说明：
  - 设置STEP名称,如调光台灯
  - OUTPUT管脚:可以设置为数字管脚,模拟管脚或者虚拟管脚,最小值设为0,最大值设为1023
  - STEP:步幅，每按一次，增加/减少一定数值
  - SEND ON RELEASE:松开发送
  - SEND STEP:发送步幅
  - LOOP VALUES:数值循环
  - ICONS:图标,可以选择箭头或加减符号

  硬件连接：
  LED连接到GPIO2

  blynk_for_chinese 库地址：https://github.com/hznupeter/blynk-library-for-chinese

  Blynk物联网学习资料：https://gitee.com/hznupeter/Blynk_IOT/wikis
*************************************************************/
#define BLYNK_PRINT Serial // 开启串口监视
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 163), 8080);
}

void loop()
{
  Blynk.run();
}

