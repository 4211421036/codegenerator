#include <Wire.h>
#include <Timer.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ArduinoJson.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <FirebaseArduino.h>
#include <Arduino.h>

MAX30105 particleSensor;

char ssid[] = "Galhan";
char pass[] = "123231";

#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
Timer t;

String dataSented;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  while(WiFi.status()!= WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("Berhasil Koneksi");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  t.every(2000, sentData);
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

}

void sentData(){
  Firebase.setString("cardionet",dataSented);
}

void loop() {
    t.update();

    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000) {
    Serial.print(" No finger?");
    dataSented = "No Finger";
    beatPerMinute = 0;
    beatAvg = 0;
  }else{
    dataSented = String(beatAvg);
  }
  Serial.println();

}
