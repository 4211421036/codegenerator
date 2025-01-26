#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include "Adafruit_VEML6070.h"

// Replace with your network credentials
const char* ssid = "Galaxy";
const char* password = "fkhw8785";

Adafruit_VEML6070 uv = Adafruit_VEML6070();
WebServer server(80);

float uvValue;

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>ESP32 UV Sensor</title>
      <style>
          body { font-family: Arial, sans-serif; background-color: #f0f0f5; margin: 0; padding: 0; }
          .container { display: grid; grid-template-columns: 1fr 1fr; grid-template-rows: auto auto 1fr; gap: 20px; padding: 20px; }
          .card { background: white; padding: 20px; border-radius: 30px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
          .card h2 { margin: 0 0 10px; }
          #chartContainer { height: 370px; width: 100%; }
          #prediction { padding: 20px; }
          .advice { padding: 20px; }
          .active { color: green; }
          .inactive { color: red; }
      </style>
  </head>
  <body>
      <div class="container">
          <div class="card" style="grid-column: 1 / span 2;">
              <h2>Hasil Pengukuran UV</h2>
              <div id="chartContainer"></div>
          </div>
          <div class="card">
              <h2>Saran</h2>
              <div id="advice" class="advice"></div>
          </div>
          <div class="card">
              <h2>Prediksi UV</h2>
              <div id="prediction" class="prediction"></div>
          </div>
      </div>
      <script src="https://canvasjs.com/assets/script/canvasjs.min.js"></script>
      <script src="https://cdn.jsdelivr.net/npm/@tensorflow/tfjs"></script>
      <script>
          var dataPoints = [];
          var uvValues = [];
          var chart = new CanvasJS.Chart("chartContainer", {
              title: { text: "Hasil Pengukuran UV" },
              axisY: { title: "UV Index" },
              data: [{ type: "line", dataPoints: dataPoints }]
          });

          async function trainModel() {
              const model = tf.sequential();
              model.add(tf.layers.dense({units: 10, activation: 'relu', inputShape: [1]}));
              model.add(tf.layers.dense({units: 1}));

              model.compile({optimizer: 'adam', loss: 'meanSquaredError'});

              const xs = tf.tensor2d(uvValues.map((val, i) => [i]), [uvValues.length, 1]);
              const ys = tf.tensor2d(uvValues, [uvValues.length, 1]);

              await model.fit(xs, ys, {epochs: 10});

              return model;
          }

          async function updatePrediction() {
              if (uvValues.length < 6) {
                  return;
              }

              const model = await trainModel();
              const inputTensor = tf.tensor2d([uvValues.length], [1, 1]);

              const prediction = model.predict(inputTensor).dataSync()[0];
              const predictionDiv = document.getElementById('prediction');
              predictionDiv.innerHTML = `<p>Predicted UV Index: ${prediction.toFixed(2)}</p>`;
          }

          function updateChart() {
              fetch('/data').then(response => response.text()).then(data => {
                  var uv = parseFloat(data);
                  dataPoints.push({ y: uv });
                  uvValues.push(uv);
                  if (dataPoints.length > 100) {
                      dataPoints.shift();
                      uvValues.shift();
                  }
                  chart.render();
                  updateAdvice(uv);
                  updatePrediction();
              });
          }

          function updateAdvice(uv) {
              var adviceDiv = document.getElementById('advice');
              adviceDiv.innerHTML = '';

              var levels = [
                  { level: 3, text: "Kurangi waktu di depan layar." },
                  { level: 6, text: "Istirahat secara teratur." },
                  { level: 8, text: "Hindari penggunaan perangkat dalam jangka waktu lama." }
              ];

              levels.forEach(l => {
                  var status = (uv >= l.level) ? 'active' : 'inactive';
                  var adviceText = `UV Level ${l.level}: <span class="${status}">${status.charAt(0).toUpperCase() + status.slice(1)}</span> - ${l.text}`;
                  adviceDiv.innerHTML += `<p>${adviceText}</p>`;
              });
          }

          setInterval(updateChart, 1000);
      </script>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleData() {
  uvValue = uv.readUV();
  String uvStr = String(uvValue);
  server.send(200, "text/plain", uvStr);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  uv.begin(VEML6070_1_T);
  
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}
