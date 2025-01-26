#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <MQ7.h>

#define WIFI_SSID "Galaxy"
#define WIFI_PASSWORD "fkhw8786"
#define BOT_TOKEN "7016160626:AAH8JK81OZRyf5ZP1llMP6t4o_AlTwhIXvE"
#define CHAT_ID "-1002137971537"

WiFiClientSecure net_ssl;
UniversalTelegramBot bot(BOT_TOKEN, net_ssl);
MQ7 mq7(32, 5.0);

// Neural Network Parameters
float weights[3] = {0.3, 0.5, 0.2}; // Initial weights for inputs
float bias = 0.1; // Bias for the neural network

// Activation function (Sigmoid)
float sigmoid(float x) {
  return 1 / (1 + exp(-x));
}

// Defuzzification function
float defuzzify(float fuzzyOutput) {
  if (fuzzyOutput < 0.5) {
    return 0; // Safe
  } else {
    return 100; // Danger
  }
}

// FNN Prediction Function
float predictFNN(float input) {
  float fuzzyInput1 = exp(-pow((input - 10) / 15, 2)); // Gaussian for low
  float fuzzyInput2 = exp(-pow((input - 40) / 15, 2)); // Gaussian for medium
  float fuzzyInput3 = exp(-pow((input - 70) / 15, 2)); // Gaussian for high
  
  float hiddenSum = fuzzyInput1 * weights[0] + fuzzyInput2 * weights[1] + fuzzyInput3 * weights[2] + bias;
  float output = sigmoid(hiddenSum);
  return defuzzify(output);
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
  net_ssl.setCACert(TELEGRAM_CERTIFICATE_ROOT);
}

void loop() {
  int coLevel = mq7.getPPM();
  Serial.print("Nilai Sensor CO: ");
  Serial.println(coLevel);

  float prediction = predictFNN(coLevel);

  String message;
  if (prediction == 0) {
    message = "Tingkat polutan CO: " + String(coLevel) + " ppm, Status: Aman.";
  } else {
    message = "Tingkat polutan CO: " + String(coLevel) + " ppm, Status: Bahaya!";
  }
  bot.sendMessage(CHAT_ID, message, "");
  delay(5000);
}
