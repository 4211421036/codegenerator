/*************************************************************
  项目说明：通过bridge可以控制外的设备

  控制端为A，被控制端为B。将B的授权码告知A，A就可以通过以下方式控制B

    bridge.digitalWrite(8, HIGH)
    bridge.digitalWrite("A0", LOW) // <- 被控制端要支持 名称管脚
    bridge.analogWrite(3, 123)
    bridge.virtualWrite(V1, "hello")

     控制端App项目设置:
     创建bridge组件，设置管脚为V1
 *************************************************************/
#define BLYNK_PRINT Serial // 开启串口监视
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码

WidgetBridge bridge1(V1);//在虚拟管脚V1创建bridge

BlynkTimer timer;

static bool value = true;
void blynkAnotherDevice()
{
  //向被控制端发送数据
  if (value) {
    bridge1.digitalWrite(12, HIGH);  //被控制端的12号管脚将被设为 HIGH
    bridge1.virtualWrite(V5, 1); // 向被控制端V5管脚发送1，被控制端需要用BLYNK_WRITE(V5) 接收

    /////////////////////////////////////////////////////////////////////////////////////////
    //特别注意，如果向被控制端的虚拟管脚发送数据，
    //被控制端需要用 BLYNK_WRITE函数来接收，类似于接收来自app端的虚拟管脚数据
    //
    //    BLYNK_WRITE(V5){
    //    int pinData = param.asInt(); // 接收来自bridge的数据
    //    }
    //
    /////////////////////////////////////////////////////////////////////////////////////////
  } else {
    bridge1.digitalWrite(12, LOW); //被控制端的12号管脚将被设为 LOW
    bridge1.virtualWrite(V5, 0); // 向被控制端V5管脚发送0，被控制端需要用BLYNK_WRITE(V5) 接收
  }
  // value取反
  value = !value;
}

BLYNK_CONNECTED() {
  bridge1.setAuthToken("OtherAuthToken"); //被控制端的授权码
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式
  timer.setInterval(1000L, blynkAnotherDevice);  // 每秒钟调用 blynkAnotherDevice 函数
}

void loop()
{
  Blynk.run();
  timer.run();
}

