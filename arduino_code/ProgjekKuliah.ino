#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>

// Replace with your network details
const char* ssid = "Galaxy";
const char* password = "fkhw8785";

#define DHTPIN 5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
float t = 0.0;
float h = 0.0;
AsyncWebServer server(80);
unsigned long previousMillis = 0;  
const long interval = 10000;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
<h2>ESP8266 & Arduino DHT Data</h2>
<div style="width:70%;margin:auto;">
  <canvas id="dhtChart"></canvas>
</div>
<script>
  var temperatureData = [];
  var humidityData = [];
  var timeLabels = [];
  var ctx = document.getElementById('dhtChart').getContext('2d');
  var dhtChart = new Chart(ctx, {
    type: 'line',
    data: {
      labels: timeLabels,
      datasets: [{
        label: 'Temperature',
        data: temperatureData,
        borderColor: 'red',
        yAxisID: 'y1'
      },{
        label: 'Humidity',
        data: humidityData,
        borderColor: 'blue',
        yAxisID: 'y2'
      }]
    },
    options: {
      scales: {
        y1: {
          beginAtZero: true
        },
        y2: {
          beginAtZero: true,
          position: 'right'
        }
      }
    }
  });

  function fetchData() {
    fetch('/data').then(response => response.json())
    .then(data => {
      if(temperatureData.length > 10) {
        // remove the oldest entry if array has more than 10 entries
        temperatureData.shift();
        humidityData.shift();
        timeLabels.shift();
      }
      temperatureData.push(data.temperature);
      humidityData.push(data.humidity);
      timeLabels.push(new Date().toLocaleTimeString());
      dhtChart.update();
    });
  }

  setInterval(fetchData, 10000); // fetch every 10 seconds
</script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"temperature\":";
    json += t;
    json += ",";
    json += "\"humidity\":";
    json += h;
    json += "}";
    request->send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    t = dht.readTemperature();
    h = dht.readHumidity();
  }
}
