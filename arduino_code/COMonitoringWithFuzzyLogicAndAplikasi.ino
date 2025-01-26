#include <MQ7.h>
#include <MQ3.h>

/*
Riset ini didanai oleh LPPM
*/
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Fuzzy.h>
#include <MQ7.h>

#define WIFI_SSID "Galaxy"
#define WIFI_PASSWORD "fkhw8786"

//#define BOT_TOKEN "6750124624:AAHJS1dpTLPQLqWoX54x2z-N3eU81HWpNyg" //Tugas Fuzzy
//#define BOT_TOKEN "6496147502:AAHpFRUPTyALtTuAiVuDHG1Vrd4T1JVumTw"  //Server FMIPA
//#define BOT_TOKEN "6699715051:AAH2B5g0LP7lr4rwyVT9PHVkXjLg_1p43YU" //Server FISIP dan FEB
#define BOT_TOKEN "7016160626:AAH8JK81OZRyf5ZP1llMP6t4o_AlTwhIXvE" //Server FH
//#define BOT_TOKEN "6497483971:AAFz2hihiSokzyHTnXt3QzvhGFYdUkLt-iY" //Server FIK
//#define BOT_TOKEN "6737299519:AAF34LnNwdhoWd9dFOxpQVLoH-IBJYn1DLE" //Server FK
//#define BOT_TOKEN "7138196159:AAENvkXlYMY_gYm-TFX2w-xnmpyThnHfSaE" //Server FBS
//#define BOT_TOKEN "6721453436:AAHbSVOyOKIBBg7rU29Z41UAD2-ZJRX53zY" //Server FIPP

#define CHAT_ID "-1002137971537"  //Channel ID

WiFiClientSecure net_ssl;
UniversalTelegramBot bot(BOT_TOKEN, net_ssl);
Fuzzy *fuzzy = new Fuzzy();
MQ7 mq7(32, 5.0);

void setup() {
  Serial.begin(9600);
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
  net_ssl.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org


  FuzzyInput *co = new FuzzyInput(1);
  FuzzySet *low = new FuzzySet(0, 10, 10, 30);
  co->addFuzzySet(low);
  FuzzySet *medium = new FuzzySet(20, 40, 40, 60);
  co->addFuzzySet(medium);
  FuzzySet *high = new FuzzySet(50, 70, 70, 1023);
  co->addFuzzySet(high);
  fuzzy->addFuzzyInput(co);

  FuzzyOutput *alert = new FuzzyOutput(1);
  FuzzySet *safe = new FuzzySet(0, 0, 0, 45);
  alert->addFuzzySet(safe);
  FuzzySet *danger = new FuzzySet(30, 60, 60, 90);
  alert->addFuzzySet(danger);
  fuzzy->addFuzzyOutput(alert);

  // Aturan fuzzy
  FuzzyRuleAntecedent *ifLowOrMedium = new FuzzyRuleAntecedent();
  ifLowOrMedium->joinWithOR(low, medium);
  FuzzyRuleConsequent *thenSafe = new FuzzyRuleConsequent();
  thenSafe->addOutput(safe);
  FuzzyRule *fRule1 = new FuzzyRule(1, ifLowOrMedium, thenSafe);
  fuzzy->addFuzzyRule(fRule1);

  FuzzyRuleAntecedent *ifHigh = new FuzzyRuleAntecedent();
  ifHigh->joinSingle(high);
  FuzzyRuleConsequent *thenDanger = new FuzzyRuleConsequent();
  thenDanger->addOutput(danger);
  FuzzyRule *fRule2 = new FuzzyRule(2, ifHigh, thenDanger);
  fuzzy->addFuzzyRule(fRule2);
}

void loop() {
  int coLevel = mq7.getPPM();
  Serial.print("Nilai Sensor CO: ");
  Serial.println(coLevel);
  delay(5000);

  fuzzy->setInput(1, coLevel);

  fuzzy->fuzzify();

  float alertLevel = fuzzy->defuzzify(1);
  if (alertLevel == 0.0) {
    // Replace the output message with an error message
    String message = "Error: Sensor not sensing";
    bot.sendMessage(CHAT_ID, message, "");
    delay(1000);
  } else {
    //String message = "Tingkat polutan CO: " + String(coLevel) + ", Tingkat bahaya: " + String(alertLevel) + " di Wilayah FMIPA";  //FMIPA
    String message = "Tingkat polutan CO: " + String(coLevel) + " ppm, Tingkat bahaya: " + String(alertLevel) + "% di Sekitaran Wilayah Fakultas Matematika dan Ilmu Pengetahuan dan Fakultas Bahasa dan Seni"; //FH
    //String message = "Tingkat polutan CO: " + String(coLevel) + ", Tingkat bahaya: " + String(alertLevel) + " di Wilayah FK"; //FK
    //String message = "Tingkat polutan CO: " + String(coLevel) + ", Tingkat bahaya: " + String(alertLevel) + " di Wilayah FISIP dan FEB"; //FISIP dan FEB
    //String message = "Tingkat polutan CO: " + String(coLevel) + ", Tingkat bahaya: " + String(alertLevel) + "di Wilayah FIK"; //FIK
    //String message = "Tingkat polutan CO: " + String(coLevel) + ", Tingkat bahaya: " + String(alertLevel) + "di Wilayah FBS"; //FBS
    //String message = "Tingkat polutan CO: " + String(coLevel) + ", Tingkat bahaya: " + String(alertLevel) + "di Wilayah FIPP"; //FIPP
    bot.sendMessage(CHAT_ID, message, "");
    delay(1000);
  }
}