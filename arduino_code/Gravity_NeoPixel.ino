/*************************************************************
  项目说明：彩灯控制
  App项目设置:  创建slider组件，管脚设置为V1，范围设置0...500
  硬件连接：WS2812连接到GPIO12管脚
  温馨提示：该项目需要下载以下第三方库
  https://github.com/adafruit/Adafruit_NeoPixel
  blynk_for_chinese 库地址：https://github.com/hznupeter/blynk-library-for-chinese

  Blynk物联网学习资料：https://gitee.com/hznupeter/Blynk_IOT/wikis
  *************************************************************/
  #define BLYNK_PRINT Serial // 开启串口监视
  #include <ESP8266WiFi.h>
  #include <BlynkSimpleEsp8266.h>
  #include <Adafruit_NeoPixel.h>
  char auth[] = "27512d11ed95407e9f5f3cb6ef6d88d8";
  char ssid[] = "ssid";
  char pass[] = "pass";

  #define PIN 12

  Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, PIN, NEO_GRB + NEO_KHZ800);

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if (WheelPos < 170) {
      WheelPos -= 85;
      return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
      } else {
        WheelPos -= 170;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
      }
    }

    BLYNK_WRITE(V1)
    {
      float R = param[0].asFloat();
      float G = param[1].asFloat();
      float B = param[2].asFloat();
      int RR = map(R, (-9.8), 9.8, 0, 255);
      int GG = map(G, (-9.8), 9.8, 0, 255);
      int BB = map(B, (-9.8), 9.8, 0, 255);
      for (int i = 0; i < strip.numPixels(); i++)
      {
        strip.setPixelColor(i, RR, GG, BB);
    // 或者: strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + shift) & 255));
  }
  strip.show();
}

void setup()
{

  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  //Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 31, 249), 8080);//自建服务器ip模式
  strip.begin();
  strip.show();
}

void loop()
{
  Blynk.run();
}

