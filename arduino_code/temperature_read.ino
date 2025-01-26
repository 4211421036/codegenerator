/*This code is an Arduino temperature monitoring system that reads data from 7 TMP36 sensors,
converts the voltage to Celsius, and displays the labeled results via serial communication.*/

#include <Arduino.h>

const int NUM_SENSORS = 7;
const int TEMP_SENSOR_PINS[NUM_SENSORS] = {A0, A1, A6, A7, A8, A9, A17}; // Analog pins connected to the TMP36 sensors
const char *SENSOR_LABELS[NUM_SENSORS] = {"OBC", "PDU", "Battery Board", "Solar Panel 1", "Solar Panel 2", "Solar Panel 3", "Solar Panel 4"};

const float MV_PER_DEGREE_F = 1.0;             // 1 mV/°F
const float OFFSET_F = 58.0;                   // 58 mV (58°F) offset in the output voltage
const float MV_PER_ADC_UNIT = 3300.0 / 1024.0; // Assuming 3.3V reference voltage and 10-bit ADC resolution

float readTemperatureCelsius(int sensorPin)
{
  int adcValue = analogRead(sensorPin);
  float voltage = adcValue * MV_PER_ADC_UNIT;

  // Convert voltage to temperature in Fahrenheit and then subtract the offset
  float temperatureF = (voltage - OFFSET_F) / MV_PER_DEGREE_F;

  // Convert temperature to Celsius
  float temperatureC = (temperatureF - 32) * 5 / 9;

  return temperatureC;
}

void setup()
{
  Serial.begin(9600); // Start the serial communication at 9600 baud rate
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    pinMode(TEMP_SENSOR_PINS[i], INPUT);
  }
}

void loop()
{
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    float temperatureC = readTemperatureCelsius(TEMP_SENSOR_PINS[i]);
    Serial.print(SENSOR_LABELS[i]);
    Serial.print(" Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" °C");
  }

  Serial.println(); // Add an empty line for better readability
  delay(5000);      // Wait for 5 seconds before reading the temperatures again
}
