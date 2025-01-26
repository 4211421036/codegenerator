#include <Firebase.h>
#include <FirebaseESP32.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>
#include <DNSServer.h>

#define FIREBASE_HOST "vehicleappssec-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "XtpiyVJ4kn3KJ9iaqwAXsbK7LoAiM45HDznXJPkc"

WiFiClientSecure net_ssl;
FirebaseData firebaseData;

void configModeWiFiCall(WiFiManager *wm) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAP("ConfigureSecures"));  // Ganti "Your_SSID" dengan SSID yang Anda inginkan
  Serial.println(wm->getConfigPortalSSID());
}

#define TR 25

bool state = false; // Variabel state untuk mengontrol relay

void setup() {
  Serial.begin(115200);
  pinMode(TR, OUTPUT);
  digitalWrite(TR, LOW);
  WiFiManager wifiManager;

  wifiManager.setCustomHeadElement(
    "<style>"
    "body { background-color: #f0f0f0; font-family: Arial, sans-serif; }"
    "h1 { color: #2f4f4f; }"
    ".form-field { margin-bottom: 1em; }"
    ".form-button { background-color: #008CBA; color: white; padding: 14px 20px; border: none; cursor: pointer; width: 100%; }"
    ".form-button:hover { opacity: 0.8; }"
    "</style>"
    "<h1 style='text-align: center;'>Konfigurasi Jaringan Mobil</h1>"
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
}

void loop() {
  if (Firebase.getString(firebaseData, "Status_of_started")) {
    String vehicleStatus = firebaseData.stringData();
    if (vehicleStatus == "detection") {
      Serial.println("Mobile Menyala");
      state = true; // Set state menjadi true ketika vehicleStatus adalah "detection"
    } else if (vehicleStatus == "null") {
      Serial.println("Mobile mati");
      state = false; // Set state menjadi false ketika vehicleStatus adalah "null"
    }
  }

  if (state) {
    digitalWrite(TR, HIGH);
  } else {
    digitalWrite(TR, LOW);
  }
}
