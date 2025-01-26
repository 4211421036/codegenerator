#include <WiFi.h>
#include <WebServer.h>

const int trigPin = 12;
const int echoPin = 13;

long duration;
float distanceMm;

// Replace with your network credentials
#define WIFI_SSID "Galaxy"
#define WIFI_PASSWORD "fkhw8785"

WebServer server(80);

String readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceMm = duration * 0.0344 / 2;

  // Prints the distance in the Serial Monitor
  Serial.print("Distance (mm): ");
  Serial.println(distanceMm);

  // Return the distance as a string
  return String(distanceMm);
}

String applyFuzzyLogic(float distance) {
  if (distance < 200) {
    return "Very Poor";
  } else if (distance < 300) {
    return "Poor";
  } else if (distance < 500) {
    return "Medium";
  } else if (distance < 800) {
    return "Healthy";
  } else if (distance < 1000) {
    return "Very Healthy";
  } else {
    return "Out of Range";
  }
}

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>ESP32 Distance Sensor</title>
      <style>
          body { font-family: Arial, sans-serif; background-color: #f0f0f5; margin: 0; padding: 0; }
          .container { display: grid; grid-template-columns: 1fr 1fr; grid-template-rows: auto auto 1fr; gap: 20px; padding: 20px; }
          .card { background: white; padding: 20px; border-radius: 30px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
          .card h2 { margin: 0 0 10px; }
          #chartContainer { height: 370px; width: 100%; }
          #prediction { padding: 20px; }
          .mitigation { padding: 20px; }
          .active { color: green; }
          .inactive { color: red; }
      </style>
  </head>
  <body>
      <div class="container">
          <div class="card" style="grid-column: 1 / span 2;">
              <h2>Mangrove Mitigation System Measurement</h2>
              <div id="chartContainer"></div>
          </div>
          <div class="card">
              <h2>Mitigation Measures</h2>
              <div id="mitigation" class="mitigation"></div>
          </div>
          <div class="card">
              <h2>Prediction for Tomorrow</h2>
              <div id="prediction" class="prediction"></div>
          </div>
      </div>
      <script src="https://canvasjs.com/assets/script/canvasjs.min.js"></script>
      <script src="https://cdn.jsdelivr.net/npm/@tensorflow/tfjs"></script>
      <script>
          var dataPoints = [];
          var chart = new CanvasJS.Chart("chartContainer", {
              title: { text: "Sensor Measurement Results" },
              axisY: { title: "Distance (mm)" },
              data: [{ type: "line", dataPoints: dataPoints }]
          });

          function updateChart() {
              fetch('/data').then(response => response.json()).then(data => {
                  var distance = parseFloat(data.distance);
                  var action = data.action;
                  dataPoints.push({ y: distance });
                  if (dataPoints.length > 100) { // Limit to 100 data points to keep the chart manageable
                      dataPoints.shift();
                  }
                  chart.render();
                  updateMitigation(distance, action);
                  updatePrediction();
              });
          }

          function updateMitigation(distance, action) {
              var mitigationDiv = document.getElementById('mitigation');
              mitigationDiv.innerHTML = '';

              var levels = [
                  { level: 200, text: "Very Poor: Carry out immediate restoration." },
                  { level: 300, text: "Poor: Monitor and make small improvements." },
                  { level: 500, text: "Medium: Monitor the condition regularly." },
                  { level: 800, text: "Healthy: Do some light maintenance." },
                  { level: 1000, text: "Very Healthy: Continue the conservation programme." }
              ];

              levels.forEach(l => {
                  var status = distance < l.level ? 'active' : 'inactive';
                  var mitigationText = `Level ${l.level} mm: <span class="${status}">${status.charAt(0).toUpperCase() + status.slice(1)}</span> - ${l.text}`;
                  mitigationDiv.innerHTML += `<p>${mitigationText}</p>`;
              });
          }

          function updatePrediction() {
              // Placeholder for ANN prediction logic
              // Simulate prediction with random data for demonstration purposes
              var prediction = Math.random() * 1000;
              var predictionDiv = document.getElementById('prediction');
              predictionDiv.innerHTML = `<p>Predicted Distance for Tomorrow: ${prediction.toFixed(2)} mm</p>`;
          }

          setInterval(updateChart, 1000);
      </script>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleData() {
  String distanceStr = readDistance();
  String actionStr = applyFuzzyLogic(distanceMm);

  String json = "{\"distance\":\"" + distanceStr + "\",\"action\":\"" + actionStr + "\"}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

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
