#include <SoftwareSerial.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 7
#define DHTTYPE DHT22
#define RELAY_PIN 8
DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial espSerial(2, 3);
LiquidCrystal_I2C lcd(0x27, 16, 2);

String localIP = "192.168.4.1";

void setup() {
  Serial.begin(115200);
  espSerial.begin(115200);
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  lcd.init();
  lcd.backlight();

  delay(1000);

  sendCommand("AT+CWMODE=2", 1000);
  sendCommand("AT+CWSAP=\"UTS\",\"12345678\",5,3", 5000);
  sendCommand("AT+CIFSR", 2000);
  sendCommand("AT+CIPMUX=1", 1000);
  sendCommand("AT+CIPSERVER=1,80", 1000);
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (temperature > 40) {
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }

  if (espSerial.available()) {
    if (espSerial.find("+IPD,")) {
      delay(1000);

      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();

      String wb = "<html><head>";
      wb += "<style>";
      wb += ".bar {height: 30px; color: white; padding: 5px; margin: 5px 0;}";
      wb += ".tempBar {background-color: red; width:" + String(temperature) + "%;}";
      wb += ".humidBar {background-color: blue; width:" + String(humidity) + "%;}";
      wb += "</style>";
      wb += "</head><body><h1>Data DHT22</h1>";

      // Grafik bar suhu
      wb += "<div class='bar tempBar'>Suhu: " + String(temperature) + " C</div>";
      // Grafik bar kelembapan
      wb += "<div class='bar humidBar'>Kelembapan: " + String(humidity) + " %</div>";

      if (temperature > 40) {
        digitalWrite(RELAY_PIN, HIGH);
        wb += "<script>alert('Suhu lebih dari 40C, relay dinyalakan!');</script>";
      } else {
        digitalWrite(RELAY_PIN, LOW);
      }

      wb += "</body></html>";

      String cipSend = "AT+CIPSEND=0," + String(wb.length());
      sendCommand(cipSend, 1000);
      espSerial.println(wb);

      delay(1000);
      sendCommand("AT+CIPCLOSE=0", 1000);
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu: " + String(temperature) + "C");
  lcd.setCursor(0, 1);
  lcd.print("Kelembapan: " + String(humidity) + "%");

  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP: " + localIP);

  delay(3000);
}

void sendCommand(String command, int delayTime) {
  espSerial.println(command);
  delay(delayTime);
  while (espSerial.available()) {
    String inData = espSerial.readStringUntil('\n');
    Serial.println(inData);

    if (command == "AT+CIFSR" && inData.startsWith("+CIFSR:STAIP,")) {
      localIP = inData.substring(12);
      localIP.trim();
    }
  }
}
