#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

MAX30105 particleSensor;
const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; // Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

const char *ssid = "*******";
const char *password = "********";

WebServer server(80);

const byte MAX_BEATS = 100; // Jumlah maksimal detak jantung yang akan disimpan
unsigned long beatTimestamps[MAX_BEATS];
byte beatValues[MAX_BEATS];
int beatCount = 0;

#define BOT_TOKEN "*************:************************************"

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  // Tampilkan alamat IP pada serial monitor
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/download", HTTP_GET, handleDownload); // New endpoint for CSV download
  server.begin();
}

void loop() {
  server.handleClient();

  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true) {
    // We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    if (beatCount < MAX_BEATS) {
      beatTimestamps[beatCount] = millis();
      beatValues[beatCount] = (byte)beatsPerMinute;
      beatCount++;

      // Cek apakah Heart Rate (BPM) melebihi ambang batas tertentu
      if (beatsPerMinute >= 100) {
        // Menghitung Denyut Nadi Normal berdasarkan usia, jika diperlukan
        String age = "30";  // Ganti dengan usia Anda
        int maxBpm = 220 - int(age);
        String message = "Detak Jantung Anda: " + String(beatsPerMinute) + " BPM\n";

        // Hitung target Heart Rate untuk aktivitas moderat dan intensitas tinggi
        int moderateMinBpm = int(maxBpm * 0.64);
        int moderateMaxBpm = int(maxBpm * 0.76);
        int vigorousMinBpm = int(maxBpm * 0.77);
        int vigorousMaxBpm = int(maxBpm * 0.93);

        message += "Target Heart Rate for Moderate Activity: " + String(moderateMinBpm) + " - " + String(moderateMaxBpm) + " BPM\n";
        message += "Target Heart Rate for Vigorous Activity: " + String(vigorousMinBpm) + " - " + String(vigorousMaxBpm) + " BPM";

        sendTelegramMessage(message);
      }
    }
    
    // Calculate BPM and AVG within the beat detection block
    beatsPerMinute = 60 / (delta / 1000.0);
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
}

void sendTelegramMessageMedically(const String &message) {
  String chatId = "YOUR_CHAT_ID";  // Ganti dengan chat ID Anda
  String markdownMode = "MarkdownV2";

  // Pesan akan diformat dalam tampilan Markdown
  String formattedMessage = "*Pemberitahuan Medis*\n\n";
  formattedMessage += message;

  // Kirim pesan dengan format Markdown
  bot.sendMessage(chatId, formattedMessage, markdownMode);
}

void handleRoot() {
String page = "<html><head><meta http-equiv=\"refresh\" content=\"1\">";
page += "<style>";
page += "body {font-family: Arial, sans-serif; background-color: #f2f2f2;}";
page += "h1 {text-align: center; background-color: #333; color: #fff; padding: 10px;}";
page += "table {border-collapse: collapse; width: 80%; margin: 0 auto; background-color: #fff; border-radius: 5px; box-shadow: 0 0 5px rgba(0, 0, 0, 0.2);}";
page += "th, td {border: 1px solid #ccc; padding: 10px; text-align: center;}";
page += "th {background-color: #f2f2f2;}";
page += ".container {display: flex; justify-content: center; padding: 20px;}";
page += ".an {position: sticky; top: 0; display: flex; flex-direction: column; background-color: #fff; padding: 10px; border-radius: 5px; box-shadow: 0 0 5px rgba(0, 0, 0, 0.2);}";
page += ".button-container {display: flex; justify-content: space-between;}";
page += ".download-button {text-align: center;}";
page += ".scrollable-data {max-height: 300px; overflow-y: auto;}";
page += ".fixed-header {position: sticky; top: 0; background-color: #fff; z-index: 1; border-radius: 5px; box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);}";
page += ".fixed-row {position: sticky; top: 40px; background-color: #fff; z-index: 1; height: 40px;}";
page += "</style>";
page += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
page += "<script>";
page += "document.addEventListener('DOMContentLoaded', function() {"; // Menunggu hingga halaman sepenuhnya dimuat
page += "  setInterval(function() {";
page += "    fetch('/data')";
page += "    .then(response => response.json())";
page += "    .then(data => {";
page += "      document.getElementById('irValue').textContent = data.irValue;";
page += "      document.getElementById('bpm').textContent = data.bpm;";
page += "      document.getElementById('avgBpm').textContent = data.avgBpm;";
page += "    });";
page += "  }, 1000);";
page += "  var ctx = document.getElementById('myChart').getContext('2d');";
page += "  var myChart = new Chart(ctx, {";
page += "    type: 'bar',";
page += "    data: {";
page += "      labels: ["; // Label timestamp di sini
for (int i = 0; i < beatCount; i++) {
  page += "'" + String(beatTimestamps[i]) + "',";
}
page += "      ],";
page += "      datasets: [{";
page += "        label: 'Heart Rate',";
page += "        data: ["; // Data BPM di sini
for (int i = 0; i < beatCount; i++) {
  page += String(beatValues[i]) + ",";
}
page += "        ],";
page += "        backgroundColor: 'rgba(75, 192, 192, 0.2)',";
page += "        borderColor: 'rgba(75, 192, 192, 1)',";
page += "        borderWidth: 1";
page += "      }]";
page += "    },";
page += "    options: {";
page += "      scales: {";
page += "        y: {";
page += "          beginAtZero: true";
page += "        }";
page += "      }";
page += "    }";
page += "  });";
page += "});"; // Penutup dari event listener 'DOMContentLoaded'
page += "</script>";
page += "</head><body>";
page += "<h3>Realtime Pengukuran</h1>";
page += "<div class='container'>";
page += "<div class='an'>";
page += "<h3>Data Pengukuran Terkini<h3>";
page += "<table>";
page += "<thead><tr><th class='fixed-header'>IR Value</th><th class='fixed-header'>BPM</th><th class='fixed-header'>Avg BPM</th></tr></thead>";
page += "<tbody>";
page += "<tr class='fixed-row'><td id='irValue'>" + String(particleSensor.getIR()) + "</td><td id='bpm'>" + String(beatsPerMinute) + "</td><td id='avgBpm'>" + String(beatAvg) + "</td></tr>";
page += "</tbody>";
page += "</table>";
page += "<div>";
page += "<h1>Grafik Analitik<h1>";
page += "<canvas id='myChart' width='400' height='200'></canvas>"; // Container untuk grafik
page += "</div>";
page += "</div>";
page += "<div class='an'>";
page += "<h3>Grafik Analitik<h3>";
page += "<table>";
page += "<thead><tr><th class='fixed-header'>Timestamp</th><th class='fixed-header'>Heart Rate</th><th class='fixed-header'><a href='/download'><button>Download</button></a></th></tr></thead>";
page += "<tbody class='scrollable-data'>";
for (int i = 0; i < beatCount; i++) {
  page += "<tr><td>" + String(beatTimestamps[i]) + "</td><td>" + String(beatValues[i]) + "</td></tr>";
}
page += "</tbody>";
page += "</table>";
page += "</div>";
page += "</div>";
page += "</body></html>";
  server.send(200, "text/html", page);
}

void handleDownload() {
  String csvData = "Timestamp,Heart Rate (BPM)\n";
  for (int i = 0; i < beatCount; i++) {
    csvData += String(beatTimestamps[i]) + "," + String(beatValues[i]) + "\n";
  }
  server.sendHeader("Content-Disposition", "attachment; filename=heart_rate_data.csv");
  server.send(200, "text/csv", csvData);
}
