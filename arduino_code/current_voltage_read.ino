/*This code monitors current and voltage of the solar panels and battery board using INA219 sensors.
It initializes and calibrates the sensors, reads the data, and prints the readings with their labels on the serial monitor */

// Include necessary libraries for communication with the INA219 sensor and I2C
#include <Wire.h>
#include <Adafruit_INA219.h>

// Define the I2C address and labels for each INA219 sensor
#define INA219_ADDR_SOLAR1 0x40  // Solar Panel 1
#define INA219_ADDR_SOLAR2 0x41  // Solar Panel 2
#define INA219_ADDR_SOLAR3 0x42  // Solar Panel 3
#define INA219_ADDR_SOLAR4 0x43  // Solar Panel 4
#define INA219_ADDR_BATTERY 0x44 // Battery Board

// Create an array of INA219 sensors and their labels
Adafruit_INA219 ina219s[] = {
    Adafruit_INA219(INA219_ADDR_SOLAR1),
    Adafruit_INA219(INA219_ADDR_SOLAR2),
    Adafruit_INA219(INA219_ADDR_SOLAR3),
    Adafruit_INA219(INA219_ADDR_SOLAR4),
    Adafruit_INA219(INA219_ADDR_BATTERY)};
const char *ina219Labels[] = {"Solar Panel 1", "Solar Panel 2", "Solar Panel 3", "Solar Panel 4", "Battery Board"};

void setup()
{
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);

  // Initialize I2C communication and configure each INA219 sensor
  for (int i = 0; i < 5; i++)
  {
    bool sensorConnected = ina219s[i].begin(&Wire2); // Initialize each INA219 sensor and check if it's connected
    if (sensorConnected)
    {
      Serial.print(ina219Labels[i]);
      Serial.println(" is connected");
    }
    else
    {
      Serial.print(ina219Labels[i]);
      Serial.println(" is not detected");
    }
    ina219s[i].setCalibration_16V_400mA(); // Set the calibration for each INA219 sensor to measure up to 16V and 400mA
  }
}

void loop()
{
  for (int i = 0; i < 5; i++)
  {
    // Read the current data and bus voltage from the INA219 sensor
    float current_mA = ina219s[i].getCurrent_mA();      // Get the current reading in milliamps
    float bus_voltage_V = ina219s[i].getBusVoltage_V(); // Get the bus voltage reading in volts

    // Print the current reading and bus voltage for each sensor with its label
    Serial.print(ina219Labels[i]); // Print the label for the current sensor
    Serial.print(" current: ");    // Print a label for the current reading
    Serial.print(current_mA);      // Print the current reading
    Serial.print(" mA");           // Print a label for the current units

    // Print the bus voltage for each sensor
    Serial.print(", bus voltage: "); // Print a label for the bus voltage reading
    Serial.print(bus_voltage_V);     // Print the bus voltage reading
    Serial.println(" V");            // Print a label for the bus voltage units
  }

  // Wait for 1 second before taking the next reading
  delay(1000); // Delay the loop for 1000 milliseconds (1 second) to control the frequency of data collection
}
