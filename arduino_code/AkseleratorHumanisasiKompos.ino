/*
Alat yang perlu dipesan
1. Wemos D1 Mini
2. Relay
3. Soil Moisture Sensor
4. DHT 22
5. Heater
6. Fun

Bismillah PKM Amli.
*/

#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "DHT.h"

#define WIFI_SSID "Galaxy"
#define WIFI_PASSWORD "fkhw8785"

#define BOT_TOKEN "7060753004:AAGewXhhGtqohZws0WXAKQ-QrJORKy0lAhU"

#define CHAT_ID "-1002151631955"

#define DHTPIN D1 //IO22
#define DHTTYPE DHT22

const int heaterPin = D2;   // IO21
const int fanPin = D3;      // IO17
const int soilMoisturePin = A0; // SVP

WiFiClientSecure net_ssl;
UniversalTelegramBot bot(BOT_TOKEN, net_ssl);
DHT dhtSensor(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);

  pinMode(heaterPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(soilMoisturePin, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  dhtSensor.begin();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());

  net_ssl.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  bot.sendMessage(CHAT_ID, "Bot telah diaktifkan dan siap menerima perintah. Ketik /start untuk melihat perintah yang tersedia.", "");
}

void loop() {
  // Baca kelembapan tanah
  int soilMoistureValue = analogRead(soilMoisturePin);
  Serial.print("Soil Moisture Value: ");
  Serial.println(soilMoistureValue);

  // Tentukan kondisi kelembapan tanah
  String soilCondition;
  float soil_ph = 7.0; // Default nilai pH netral
  if (soilMoistureValue < 300) {
    soilCondition = "Dry";
    soil_ph += 0.5; // Menambahkan 0.5 untuk kondisi tanah kering
  } else if (soilMoistureValue >= 300 && soilMoistureValue < 700) {
    soilCondition = "Moist";
    // Nilai pH tetap netral
  } else {
    soilCondition = "Wet";
    soil_ph -= 0.5; // Mengurangi 0.5 untuk kondisi tanah basah
  }
  Serial.print("Soil Condition: ");
  Serial.println(soilCondition);
  Serial.print("Adjusted Soil pH: ");
  Serial.println(soil_ph, 1);

  float temperature = readTemperature();
  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  if (temperature < 40.0) { // Set point temperature to turn on the heater
    digitalWrite(heaterPin, LOW);
    bot.sendMessage(CHAT_ID, "Suhu terlalu rendah! Pemanas dinyalakan.", "");
  } else if (temperature >= 40.0) { // Set point temperature to turn off the heater
    digitalWrite(heaterPin, HIGH);
    bot.sendMessage(CHAT_ID, "Suhu cukup hangat. Pemanas dimatikan.", "");
  }

  // Periksa pesan dari bot
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while (numNewMessages) {
    Serial.println("Got response");
    handAKMMes(numNewMessages, soil_ph, temperature);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}

float readTemperature() {
  return dhtSensor.readTemperature();
}

void handAKMMes(int numNewMessages, float pH, float temperature) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    if (text == "/start@AkseleratorHumanisasiKomposBot") {
      String welcome_message = 
          "Selamat datang di grup! Berikut adalah perintah yang tersedia:\n"
          "/kondisi - Menampilkan kondisi suhu dan pH saat ini\n"
          "/on - Menyalakan pemanas\n";
      bot.sendMessage(chat_id, welcome_message, "");
    }else if (text == "/start") {
      String welcome_message = 
          "Selamat datang di grup! Berikut adalah perintah yang tersedia:\n"
          "/kondisi - Menampilkan kondisi suhu dan pH saat ini\n"
          "/on - Menyalakan pemanas\n";
      bot.sendMessage(chat_id, welcome_message, "");
    }else if (text == "/kondisi") {
      String kondisi_suhu = (temperature > 40.0) ? "Terlalu panas" : (temperature < 20.0) ? "Terlalu dingin" : "Normal";
      String kondisi_pH = (pH < 7) ? "Asam" : (pH > 7) ? "Basa" : "Netral";
      String response = "Kondisi suhu terkini: " + kondisi_suhu + "\n"
                        "Kondisi suhu: " + String(temperature) + "°C\n"
                        "Kondisi pH: " + String(pH) + "\n";
      bot.sendMessage(chat_id, response, "");
    } else if (text == "/kondisi@AkseleratorHumanisasiKomposBot") {
      String kondisi_suhu = (temperature > 40.0) ? "Terlalu panas" : (temperature < 20.0) ? "Terlalu dingin" : "Normal";
      String kondisi_pH = (pH < 7) ? "Asam" : (pH > 7) ? "Basa" : "Netral";
      String response = "Kondisi suhu terkini: " + kondisi_suhu + "\n"
                        "Kondisi suhu: " + String(temperature) + "°C\n"
                        "Kondisi pH: " + String(pH) + "\n";
      bot.sendMessage(chat_id, response, "");
    }else if (text == "/on") {
      digitalWrite(heaterPin, LOW);
      bot.sendMessage(chat_id, "Pemanas dinyalakan!", "");
    }
    else if (text == "/off") {
      digitalWrite(heaterPin, HIGH);
      bot.sendMessage(chat_id, "Pemanas dimatikan!", "");
    }
    else if (text == "/on@AkseleratorHumanisasiKomposBot") {
      digitalWrite(heaterPin, LOW);
      bot.sendMessage(chat_id, "Pemanas dinyalakan!", "");
    }
  }
}
