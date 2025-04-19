/*
LPPM
UKMP
*/

#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define WIFI_LPPM "MIPA"
#define PW "LPPM"

#define bot_lppm "6496147502:AAHpFRUPTyALtTuAiVuDHG1Vrd4T1JVumTw"
#define channel_id "MonitorCOUNNES"

#define MQ7PIN 12

WiFiClientSecure net_ssl;
UniversalTelegramBot bot(BOT_TOKEN, net_ssl);

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
