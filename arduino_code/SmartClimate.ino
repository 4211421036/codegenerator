#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"

#define WIFI_SSID "Galaxy"
#define WIFI_PASSWORD "fkhw8785"
#define DHTPIN 4
#define DHTTYPE DHT22

const int ldr = 2;
const int rain = 15;

DHT dhtSensor(DHTPIN, DHTTYPE);
WebServer server(80);

struct SensorData {
  float temperature;
  float humidity;
  int ldr;
  int rain;
};

SensorData sensorDataArray[100];
int dataCount = 0;

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE HTML>
  <html>
  <head>
      <title>ESP32 DL Web App</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <script src="https://cdn.jsdelivr.net/npm/@tensorflow/tfjs"></script>
      <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
      <style>
          body {
              font-family: 'Poppins', sans-serif;
              margin: 0;
              padding: 20px;
              background-color: #f7f8fa;
              color: #333;
              line-height: 1.6;
          }

          h2 {
              color: #222;
              font-size: 1.8em;
              font-weight: 600;
              margin-bottom: 20px;
          }

          label {
              display: block;
              font-size: 1em;
              margin-bottom: 10px;
              color: #444;
          }

          select, button {
              width: 100%;
              max-width: 300px;
              padding: 10px;
              margin-bottom: 20px;
              font-size: 1em;
              border-radius: 8px;
              border: 1px solid #ddd;
              box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
              transition: all 0.3s ease;
          }

          select:focus, button:focus {
              outline: none;
              border-color: #007bff;
              box-shadow: 0 0 5px rgba(0, 123, 255, 0.5);
          }

          button {
              background-color: #007bff;
              color: #fff;
              cursor: pointer;
          }

          button:hover {
              background-color: #0056b3;
          }

          .charts {
              display: flex;
              flex-wrap: wrap;
              justify-content: space-between;
              gap: 20px;
              margin-top: 20px;
          }

          canvas {
              background-color: #fff;
              border-radius: 12px;
              padding: 15px;
              box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
              width: 100%;
              max-width: 48%;
          }

          .prediction-section {
              margin-top: 20px;
              background-color: #fff;
              padding: 20px;
              border-radius: 12px;
              box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
          }

          .prediction-section p {
              font-size: 1.1em;
              margin-bottom: 10px;
          }

          .training-info {
              margin-top: 20px;
              font-size: 0.9em;
              color: #777;
          }

          /* Responsive Design */
          @media (max-width: 768px) {
              h2 {
                  font-size: 1.5em;
              }

              .charts {
                  flex-direction: column;
              }

              canvas {
                  flex-basis: 100%;
                  max-width: 100%;
              }
          }
      </style>
  </head>
  <body>
      <h2>ESP32 Deep Learning Web App</h2>
      <label for="days">Choose prediction period:</label>
      <select id="days">
          <option value="1">1 Day</option>
          <option value="2">2 Days</option>
          <option value="3">3 Days</option>
          <option value="4">4 Days</option>
          <option value="5">5 Days</option>
          <option value="6">6 Days</option>
          <option value="7">1 Week</option>
      </select>
      <button onclick="getPrediction()">Get Prediction</button>

      <div class="prediction-section">
        <p>Temperature Prediction: <span id="tempPrediction">N/A</span> °C</p>
        <p>Humidity Prediction: <span id="humPrediction">N/A</span> %</p>
        <p>Light Intensity Prediction: <span id="ldrPrediction">N/A</span></p>
        <p>Rain Prediction: <span id="rainPrediction">N/A</span></p>
        <p>Overall Prediction: <span id="overallPrediction">N/A</span></p>
      </div>

      <div class="charts">
        <canvas id="temperatureChart"></canvas>
        <canvas id="humidityChart"></canvas>
      </div>

      <script>
          let sensorDataArray = [];
          let temperatureChart = null;
          let humidityChart = null;

          function initializeCharts() {
              const ctxTemp = document.getElementById('temperatureChart').getContext('2d');
              const ctxHum = document.getElementById('humidityChart').getContext('2d');

              temperatureChart = new Chart(ctxTemp, {
                  type: 'line',
                  data: {
                      labels: [],
                      datasets: [
                          {
                              label: 'Actual Temperature (°C)',
                              data: [],
                              borderColor: 'blue',
                              fill: false,
                          },
                          {
                              label: 'Predicted Temperature (°C)',
                              data: [],
                              borderColor: 'red',
                              fill: false,
                          }
                      ]
                  },
                  options: {
                      responsive: true,
                      maintainAspectRatio: false,
                      title: {
                          display: true,
                          text: 'Temperature Prediction'
                      },
                      scales: {
                          x: {
                              ticks: {
                                  autoSkip: true,
                                  maxTicksLimit: 10
                              }
                          },
                          xAxes: [{
                              type: 'time',
                              time: {
                                  unit: 'minute'
                              }
                          }],
                          yAxes: [{
                              ticks: {
                                  beginAtZero: true
                              }
                          }]
                      }
                  }
              });

              humidityChart = new Chart(ctxHum, {
                  type: 'line',
                  data: {
                      labels: [],
                      datasets: [
                          {
                              label: 'Actual Humidity (%)',
                              data: [],
                              borderColor: 'blue',
                              fill: false,
                          },
                          {
                              label: 'Predicted Humidity (%)',
                              data: [],
                              borderColor: 'red',
                              fill: false,
                          }
                      ]
                  },
                  options: {
                      responsive: true,
                      maintainAspectRatio: false,
                      x: {
                          ticks: {
                              autoSkip: true,
                              maxTicksLimit: 10
                          }
                      },
                      title: {
                          display: true,
                          text: 'Humidity Prediction'
                      },
                      scales: {
                          xAxes: [{
                              type: 'time',
                              time: {
                                  unit: 'minute'
                              }
                          }],
                          yAxes: [{
                              ticks: {
                                  beginAtZero: true
                              }
                          }]
                      }
                  }
              });
          }

          async function getData() {
              const response = await fetch('/data');
              const data = await response.json();
              const currentTime = new Date();

              sensorDataArray.push([data.temperature, data.humidity, data.ldr, data.rain]);

              // Update actual data in charts
              temperatureChart.data.labels.push(currentTime);
              temperatureChart.data.datasets[0].data.push(data.temperature);

              humidityChart.data.labels.push(currentTime);
              humidityChart.data.datasets[0].data.push(data.humidity);

              temperatureChart.update();
              humidityChart.update();

              if (sensorDataArray.length >= 6) {
                  const inputData = sensorDataArray.slice(-6);
                  const input = tf.tensor2d(inputData);

                  const model = tf.sequential();
                  model.add(tf.layers.dense({units: 5, inputShape: [4], activation: 'relu'}));
                  model.add(tf.layers.dense({units: 4, activation: 'linear'})); 
                  model.compile({optimizer: 'adam', loss: 'meanSquaredError'});

                  await model.fit(input, input);

                  const prediction = model.predict(input);
                  const predictionArray = await prediction.array();

                  // Update prediction values in UI
                  document.getElementById("tempPrediction").innerText = predictionArray[0][0].toFixed(2);
                  document.getElementById("humPrediction").innerText = predictionArray[0][1].toFixed(2);

                  // Update predicted data in charts
                  temperatureChart.data.datasets[1].data.push(predictionArray[0][0]);
                  humidityChart.data.datasets[1].data.push(predictionArray[0][1]);

                  temperatureChart.update();
                  humidityChart.update();
              }
          }

          async function getPrediction() {
              const days = document.getElementById("days").value;
              const requiredDataPoints = days * 10; // Mengasumsikan 10 data per hari

              if (sensorDataArray.length >= requiredDataPoints) {
                  const inputData = sensorDataArray.slice(-requiredDataPoints);
                  const input = tf.tensor2d(inputData);

                  const model = tf.sequential();
                  model.add(tf.layers.dense({units: 5, inputShape: [4], activation: 'relu'}));
                  model.add(tf.layers.dense({units: 4, activation: 'linear'}));
                  model.compile({optimizer: 'adam', loss: 'meanSquaredError'});

                  await model.fit(input, input);

                  const prediction = model.predict(input);
                  const predictionArray = await prediction.array();

                  const overallPrediction = (predictionArray[0][0] + predictionArray[0][1] + predictionArray[0][2] + predictionArray[0][3]) / 4;
                  document.getElementById("overallPrediction").innerText = overallPrediction.toFixed(2);
              } else {
                  alert('Not enough data for accurate overall prediction. However, current prediction is based on available data.');
              }
          }

          initializeCharts();
          setInterval(getData, 10000); // Ambil data setiap 10 detik
      </script>
  </body>
  </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handleData() {
  float temperature = dhtSensor.readTemperature();
  float humidity = dhtSensor.readHumidity();
  int ldrRead = analogRead(ldr);
  int rainRead = analogRead(rain);

  if (dataCount < 100) {
    sensorDataArray[dataCount] = {temperature, humidity, ldrRead, rainRead};
    dataCount++;
  }

  String json = "{\"temperature\":" + String(temperature) + 
                ",\"humidity\":" + String(humidity) + 
                ",\"ldr\":" + String(ldrRead) + 
                ",\"rain\":" + String(rainRead) + "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  pinMode(ldr, INPUT);
  pinMode(rain, INPUT);
  dhtSensor.begin();

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}
