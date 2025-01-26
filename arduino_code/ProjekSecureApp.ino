#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <WiFiManager.h>
#include <DNSServer.h>

//#define WIFI_SSID "Galaxy"
//#define WIFI_PASSWORD "fkhw8785"

#define BOT_TOKEN "7016160626:AAH8JK81OZRyf5ZP1llMP6t4o_AlTwhIXvE" //Server FH
#define CHAT_ID "-1002137971537"  //Channel ID

#define LED_PIN 12

WiFiClientSecure net_ssl;
UniversalTelegramBot bot(BOT_TOKEN, net_ssl);

void configModeWiFiCall(WiFiManager *wm) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAP("ConfigureSecures"));  // Ganti "Your_SSID" dengan SSID yang Anda inginkan
  Serial.println(wm->getConfigPortalSSID());
}

void setup() {
  /*
  // Koneksi ke WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);                              // Menunnggu atau delay waktu untuk koneksi ke jaringan WiFi
    Serial.println("Connecting to WiFi...");  // Pesan koneksi WiFi
  }
  Serial.println("Connected to WiFi");  // Status koneksi yang ditampilkan jika dikoneksi
  // Tampilkan alamat IP pada serial monitor
  Serial.print("Alamat IP: ");                   // Alamat IP yang digunakan untuk komunikasi via Telegram
  Serial.println(WiFi.localIP());                // Melihat Alamat IP
  */
  WiFiManager wifiManager;

  // Customise of promise render element UI
  wifiManager.setCustomHeadElement(
    "<style>"
    "body { background-color: #f0f0f0; font-family: Arial, sans-serif; }"
    "h1 { color: #2f4f4f; }"
    ".form-field { margin-bottom: 1em; }"
    ".form-button { background-color: #008CBA; color: white; padding: 14px 20px; border: none; cursor: pointer; width: 100%; }"
    ".form-button:hover { opacity: 0.8; }"
    "</style>"
    "<h1 style='text-align: center;'>WiFi Configuration</h1>"
  );
  wifiManager.setAPCallback(configModeWiFiCall);
  if(!wifiManager.autoConnect("ConfigureSecure")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }
  //if you get here you have connected to the WiFi
  Serial.println(F("WIFIManager connected!"));
 
  Serial.print(F("IP --> "));
  Serial.println(WiFi.localIP());
  Serial.print(F("GW --> "));
  Serial.println(WiFi.gatewayIP());
  Serial.print(F("SM --> "));
  Serial.println(WiFi.subnetMask());
 
  Serial.print(F("DNS 1 --> "));
  Serial.println(WiFi.dnsIP(0));
 
  Serial.print(F("DNS 2 --> "));
  Serial.println(WiFi.dnsIP(1)); 
  net_ssl.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org

}

void loop() {
   int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while(numNewMessages) {
    Serial.println("got response");
    for (int i=0; i<numNewMessages; i++) {
      String chat_id = String(bot.messages[i].chat_id);
      if (chat_id == CHAT_ID) {
        String text = bot.messages[i].text;
        if (text == "LED ON") {
          digitalWrite(LED_PIN, HIGH);  // Menyalakan LED
          bot.sendMessage(CHAT_ID, "LED telah dinyalakan");
        } else if (text == "LED OFF") {
          digitalWrite(LED_PIN, LOW);  // Mematikan LED
          bot.sendMessage(CHAT_ID, "LED telah dimatikan");
        }
      }
    }
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }

}
