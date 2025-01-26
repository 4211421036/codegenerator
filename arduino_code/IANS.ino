#include <WiFi.h>
#include <FirebaseESP32.h>
#include <WebServer.h>

#define FIREBASE_HOST "ukmpapps-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyAdgRFCXdXOBlYBm20MLWKeV2FYvUUzDP0"

#define WIFI_SSID "Galaxy A32AA2A"
#define WIFI_PASSWORD "fkhw8785"

const int soilPin = 34;
WebServer server(80);
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  float soilMoisture = readSoilMoisture();
  sendToFirebase(soilMoisture);
  String html = "<html><head><script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script><script type=\"text/javascript\">google.charts.load('current', {'packages':['corechart']});google.charts.setOnLoadCallback(drawChart);function drawChart() {var data = google.visualization.arrayToDataTable([['Time', 'Soil Moisture'],";
  html += getDataFromFirebase();
  html += "]);var options = {title: 'Soil Moisture Data',curveType: 'function',legend: { position: 'bottom' }};var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));chart.draw(data, options);}</script></head><body><div id=\"curve_chart\" style=\"width: 900px; height: 500px\"></div></body></html>";
  server.send(200, "text/html", html);

  // Send data to Firebase to be displayed at ukmpapps.web.app
  Firebase.pushFloat(firebaseData, "/soilMoisture", soilMoisture);
}

float readSoilMoisture() {
  int soilValue = analogRead(soilPin);
  float soilMoisture = map(soilValue, 4095, 0, 0, 100);
  soilMoisture = constrain(soilMoisture, 0, 100);
  return soilMoisture;
}

void sendToFirebase(float soilMoisture) {
  Firebase.setFloat(firebaseData, "/soilMoisture", soilMoisture);
}

String getDataFromFirebase() {
  String data = "";
  if (Firebase.getString(firebaseData, "/soilMoisture")) {
    data += "['";
    data += firebaseData.stringData();
    data += "', ";
    Firebase.getString(firebaseData, "/soilMoisture/time");
    data += firebaseData.stringData();
    data += "], ";
  }
  return data;
}
