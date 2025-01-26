/*************************************************************
  项目说明：table组件高级用法
  App项目设置:
  创建table组件，输入管脚设置为V1
  创建button组件，输出管脚设置为V10，push模式
  创建button组件，输出管脚设置为V11，push模式
 *************************************************************/
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "2a365b624c0f4ea891256d4a66d428f7";//授权码
char ssid[] = "ssid";//wifi名称
char pass[] = "psssword";//wifi密码
WidgetTable table;//创建table组件
BLYNK_ATTACH_WIDGET(table, V1);//将table与V1绑定

int rowIndex = 0;

//V10的按钮用于增加行
BLYNK_WRITE(V10) {
  if (param.asInt()) {
    table.addRow(rowIndex, "Test row", millis() / 1000);
    table.pickRow(rowIndex);
    rowIndex++;
  }
}

// V11的按钮用于清空表格
BLYNK_WRITE(V11) {
  if (param.asInt()) {
    table.clear();
    rowIndex = 0;
  }
}

void setup()
{
  Serial.begin(9600);
  // Blynk.begin(auth, ssid, pass);//官方服务器
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8080);//自建服务器域名模式
  Blynk.begin(auth, ssid, pass, IPAddress(192, 168, 1, 158), 8080);//自建服务器ip模式

  // Setup table event callbacks
  table.onOrderChange([](int indexFrom, int indexTo) {
    Serial.print("Reordering: ");
    Serial.print(indexFrom);
    Serial.print(" => ");
    Serial.print(indexTo);
    Serial.println();
  });

  table.onSelectChange([](int index, bool selected) {
    Serial.print("Item ");
    Serial.print(index);
    Serial.print(selected ? " marked" : " unmarked");
  });
}

void loop()
{
  Blynk.run();
}

