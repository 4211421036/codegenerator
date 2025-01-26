#include <WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* server = "api.gis.co.id";
const char* apiKey = "your_API_KEY";
const char* layerId = "your_LAYER_ID";

WiFiClientSecure client;

float pHValue = 0;
float soilTempValue = 0;
float soilMoistureValue = 0;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Membaca nilai pH dari sensor dan mengkonversi menjadi nilai pH
    pHValue = analogRead(A0) * (5.0 / 1023.0) * 3.5;
    // Membaca nilai suhu dari sensor dan mengkonversi menjadi nilai suhu dalam derajat Celsius
    soilTempValue = (analogRead(A1) / 4096.0 * 3300.0 - 500.0) / 10.0;
    // Membaca nilai kelembapan dari sensor dan mengkonversi menjadi persentase kelembapan tanah
    soilMoistureValue = (1 - (analogRead(A2) / 4096.0)) * 100;

    // Menyiapkan data yang akan dikirim ke webgist
    String data = "pH=" + String(pHValue, 2) + "&soil_temp=" + String(soilTempValue, 2) + "&soil_moisture=" + String(soilMoistureValue, 2);
    String url = "/v1/layers/" + String(layerId) + "/features"; // URL API endpoint untuk layer

    if (!client.connect(server, 443)) {
      Serial.println("Koneksi HTTPS gagal");
      return;
    }
    
    String request = "POST " + url + " HTTP/1.1\r\n" +
                     "Host: " + String(server) + "\r\n" +
                     "Authorization: Token " + String(apiKey) + "\r\n" +
                     "Content-Type: application/x-www-form-urlencoded\r\n" +
                     "Content-Length: " + String(data.length()) + "\r\n" +
                     "Connection: close\r\n\r\n" +
                     data + "\r\n";
                     
    client.print(request);
    delay(1000);
    client.stop();
    Serial.println("Data berhasil dikirim ke webgist GIS.co.id");
  } else {
    Serial.println("Koneksi WiFi gagal");
  }
  delay(60000);
}
