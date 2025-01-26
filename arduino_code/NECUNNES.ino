#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

// Set your network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const int sensorPin = 34; // Analog pin for ESP32
int sensorValue = 0;
float voltage = 0.0;
float glucoseLevel = 0.0;

const float intercept = 70;
const float slope = 30;

// JSON data
String jsonString;

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize the WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize the server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    sensorValue = analogRead(sensorPin);
    voltage = sensorValue * (3.3 / 4095.0);

    if (voltage < 2.25) {
      glucoseLevel = (voltage * slope) + intercept;
    } else if (voltage >= 2.9) {
      glucoseLevel = (voltage * slope) + intercept + 20;
    }

    JSONVar jsonData;
    jsonData["glucoseLevel"] = glucoseLevel;
    jsonData["status"] = (glucoseLevel >= 70 && glucoseLevel <= 140) ? "Normal" : "Abnormal";
    jsonString = JSON.stringify(jsonData);
    
    request->send(200, "application/json", jsonString);
  });

  // Start server
  server.begin();
}

void loop() {
  // Nothing here, everything is handled in the server callbacks
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 Glucose Monitor</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    body { font-family: Arial; text-align: center; }
    .status { font-size: 2rem; margin-top: 20px; }
  </style>
</head>
<body>
  <h1>ESP32 Glucose Monitor</h1>
  <canvas id="glucoseChart" width="400" height="200"></canvas>
  <div class="status" id="status"></div>
  <script>
    var ctx = document.getElementById('glucoseChart').getContext('2d');
    var chart = new Chart(ctx, {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          label: 'Glucose Level (mg/dL)',
          backgroundColor: 'rgba(255, 99, 132, 0.2)',
          borderColor: 'rgba(255, 99, 132, 1)',
          data: []
        }]
      },
      options: {
        responsive: true,
        scales: {
          x: {
            type: 'realtime',
            realtime: {
              delay: 2000,
              onRefresh: function(chart) {
                fetch('/data')
                  .then(response => response.json())
                  .then(data => {
                    var time = new Date();
                    chart.data.labels.push(time.toLocaleTimeString());
                    chart.data.datasets[0].data.push(data.glucoseLevel);
                    document.getElementById('status').innerText = `Status: ${data.status}`;
                    chart.update('quiet');
                  });
              }
            }
          }
        }
      }
    });
  </script>
</body>
</html>
)rawliteral";
