#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "MLP.h"

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

Adafruit_BMP280 bmp;
int Neurons[] = {4, 30, 20, 10, 1};
int Activations[] = {SIGMOID, SIGMOID, SIGMOID, SIGMOID};
MLP Net(5, Neurons, 1);

void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 Forced Mode Test."));
  
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or try a different address!"));
    while (1) delay(10);
  }
  
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED, Adafruit_BMP280::SAMPLING_X2, Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16, Adafruit_BMP280::STANDBY_MS_500);
  Net.begin();
}

void loop() {
  if (bmp.takeForcedMeasurement()) {
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure();
    float altitude = bmp.readAltitude(1013.25);
    
    float input_data[] = {temperature, pressure, altitude, 1};
    Net.processDataset(input_data);
    Net.propagateNet(); // Propagate through the network
    
    float prediction = Net.predict();
    float target = 1.0; // Set your target value here
    float error = target - prediction;
    Net.getError(&error); // Update the error
    
    Serial.print(F("Temperature = "));
    Serial.print(temperature);
    Serial.println(" *C");
    Serial.print(F("Pressure = "));
    Serial.print(pressure);
    Serial.println(" Pa");
    Serial.print(F("Approx altitude = "));
    Serial.print(altitude);
    Serial.println(" m");
    Serial.print(F("Prediction = "));
    Serial.println(prediction);
    Serial.print(F("Error = "));
    Serial.println(error);
    Serial.println();
    
    delay(2000);
  } else {
    Serial.println("Forced measurement failed!");
  }
}