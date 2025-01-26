/*************************************************************
  项目说明：用按钮控制LED亮灭

  App项目设置:
  创建Button组件，输出管脚设置为GPIO2,模式设为switch

  Button组件设置说明：
  - 设置Button名称，如客厅灯，台灯
  - OUTPUT管脚：可以设置为数字管脚，模拟管脚或者虚拟管脚
  - MODE：PUSH-轻触开关，按下有效，松开无效；SWITCH：切换开关，按下有效，再按一次无效
  - ON/OFF LABELS：开关文本，比如可以把OFF的文本设置为“停止”；把ON的文本设置成“运转”。
  - DESIGN：字体大小选择，文本颜色选择
  硬件连接：
  LED连接到GPIO2

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
