/*This code performs a check of the components located on the PDU and reports their status.*/

// Include necessary libraries for communication with the INA219 sensor and I2C
#include <Wire.h>
#include <Adafruit_INA219.h>

// Define the analog input pin for the TMP36 sensor and its label
const int sensorPin = 1;                        // Analog pin for the temperature sensor
const char *sensorLabel = "Temperature Sensor"; // Label for the sensor

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
        if (ina219s[i].begin(&Wire2))
        {                                          // Initialize each INA219 sensor
            ina219s[i].setCalibration_16V_400mA(); // Set the calibration for each INA219 sensor to measure up to 16V and 400mA
        }
    }

    // Wait for the serial monitor to open
    while (!Serial)
    {
        delay(10000);
    }
}

void loop()
{
    int sensorValue = analogRead(sensorPin); // Read the analog value from the sensor pin

    if (sensorValue > 15)
    { // this needs a threshold since we read an analog value even when no sensor is connected
        Serial.print(sensorLabel);
        Serial.println(" is connected and reading data.");
    }
    else
    {
        Serial.print(sensorLabel);
        Serial.println(" is not connected or not reading data.");
    }

    Serial.println(); // Add a blank line between readings

    for (int i = 0; i < 5; i++)
    {
        float current_mA = ina219s[i].getCurrent_mA(); // Get the current reading in milliamps

        if (current_mA != 0)
        {
            Serial.print(ina219Labels[i]); // Print the label for the current sensor
            Serial.println(" current sensor is connected and giving data.");
        }
        else
        {
            Serial.print(ina219Labels[i]); // Print the label for the current sensor
            Serial.println(" current sensor is not connected or not giving data.");
        }
    }

    // Wait for 10 seconds before taking the next reading
    delay(10000); // Delay the loop for 10000 milliseconds (10 seconds) to control the frequency of data collection
}
