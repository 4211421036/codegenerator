/*
Develop for delegation UKMP
Name : GALIH RDIHO UTOMO
Co-Research: Fajar and Hanina
*/
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Galaxy A32AA2A";
const char* password = "fkhw8785";

const int sensorPin = 34;

WebServer server(80);

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  pinMode(sensorPin, INPUT);

 // Koneksi ke jaringan WiFi
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi terhubung");
  
  // Tampilkan alamat IP pada serial monitor
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
  server.on("/", handleRoot);
  Serial.println("Server web dijalankan!");
}

void loop() {
  server.handleClient();
  updateData();
}

void updateData() {
  static unsigned long lastUpdate = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - lastUpdate >= 5000) {
    int sensorValue = analogRead(sensorPin);
    int soilMoisture = (sensorValue / 100);
    float pHvalue = ((sensorValue % 100) / 10.0);
    float TDSvalue = (sensorValue % 10) * 100;
    float temperature = (sensorPin * 5.0 / 1023.0 - 0.5) * -100;  // Menghitung suhu dalam derajat Celsius
    Serial.print("Soil moisture: ");
    Serial.print(soilMoisture);
    Serial.print("%, pH: ");
    Serial.print(pHvalue);
    Serial.print(", TDS: ");
    Serial.print(TDSvalue);
    Serial.print(", Temperature: ");
    Serial.print(temperature);
    Serial.println(" derajat Celsius");
    delay(1000);  // Jeda selama 1 detik
  }
}

void handleRoot() {
  int sensorValue = analogRead(sensorPin);
  int soilMoisture = (sensorValue / 100);
  float pHvalue = ((sensorValue % 100) / 10.0);
  float TDSvalue = (sensorValue % 10) * 100;
  float temperature = (sensorPin * 5.0 / 1023.0 - 0.5) * -100;  // Menghitung suhu dalam derajat Celsius
    
  String html = "<html><head><script src=\"https://cdn.plot.ly/plotly-latest.min.js\"></script></head><body>";
  html += "<div id=\"chart1\"></div>";
  html += "<script>";
  html += "var data = [{ x: [], y: [], type: 'scatter', mode: 'lines+markers', name: 'Kelembaban Tanah' }];";
  html += "var layout = { title: 'Grafik Kelembaban Tanah', xaxis: { title: 'Waktu' }, yaxis: { title: 'Kelembaban Tanah (%)' } };";
  html += "var graph = Plotly.newPlot('chart1', data, layout);";
  html += "setInterval(function() {";
  html += "var x = new Date().getTime();";
  html += "var y1 = " + String(soilMoisture) + ";";
  html += "Plotly.extendTraces('chart1', { x: [[x]], y: [[y1]] }, [0]);";
  html += "}, 5000);";
  html += "</script>";

  html += "<div id=\"chart2\"></div>";
  html += "<script>";
  html += "var data = [{ x: [], y: [], type: 'scatter', mode: 'lines+markers', name: 'pH Tanah' }];";
  html += "var layout = { title: 'Grafik pH Tanah', xaxis: { title: 'Waktu' }, yaxis: { title: 'pH Tanah' } };";
  html += "var graph = Plotly.newPlot('chart2', data, layout);";
  html += "setInterval(function() {";
  html += "var x = new Date().getTime();";
  html += "var y1 = " + String(pHvalue) + ";";
  html += "Plotly.extendTraces('chart2', { x: [[x]], y: [[y1]] }, [0]);";
  html += "}, 5000);";
  html += "</script>";

  html += "<div id=\"chart3\"></div>";
  html += "<script>";
  html += "var data = [{ x: [], y: [], type: 'scatter', mode: 'lines+markers', name: 'Suhu Tanah' }];";
  html += "var layout = { title: 'Grafik Suhu Tanah', xaxis: { title: 'Waktu' }, yaxis: { title: 'Suhu Tanah (Celsius)' } };";
  html += "var graph = Plotly.newPlot('chart3', data, layout);";
  html += "setInterval(function() {";
  html += "var x = new Date().getTime();";
  html += "var y1 = " + String(temperature) + ";";
  html += "Plotly.extendTraces('chart3', { x: [[x]], y: [[y1]] }, [0]);";
  html += "}, 5000);";
  html += "</script>";

  html += "<div id=\"chart4\"></div>";
  html += "<script>";
  html += "var data = [{ x: [], y: [], type: 'scatter', mode: 'lines+markers', name: 'TDS' }];";
  html += "var layout = { title: 'Grafik TDS Tanah', xaxis: { title: 'Waktu' }, yaxis: { title: 'TDS Tanah (ppm)' } };";
  html += "var x = [];";
  html += "var y = [];";
  html += "var graph = Plotly.newPlot('chart4', data, layout);";
  html += "setInterval(function() {";
  html += "  var time = new Date().getTime();";
  html += "  TDSvalue = " + String(TDSvalue) + ";";
  html += "  x.push(time);";
  html += "  y.push(TDSvalue);";
  html += "  var update = { x: [x], y: ["+ String(TDSvalue) + "] };";
  html += "  Plotly.update('chart4', update, layout);";
  html += "}, 5000);";
  html += "</script>";


  html += "</body></html>";

  server.send(200, "text/html", html);
}
