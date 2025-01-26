/*This code performs a system check of the components on the Artemis Cubesat and reports their status.
This includes the Adafruit Mini GPS PA1010D module, an Adafruit LSM6DSOX + LIS3MDL Inertial Measurement Unit (IMU),
TMP36FSZ temperature sensors, and INA219 current/voltage sensors, RFM23 radio, and Raspberry Pi Zero. */

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GPS.h>
#include <RH_RF22.h>
#include <RHHardwareSPI1.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_Sensor.h>

// Define the I2C address and labels for each current sensor
#define INA219_ADDR_SOLAR1 0x40  // Solar Panel 1
#define INA219_ADDR_SOLAR2 0x41  // Solar Panel 2
#define INA219_ADDR_SOLAR3 0x42  // Solar Panel 3
#define INA219_ADDR_SOLAR4 0x43  // Solar Panel 4
#define INA219_ADDR_BATTERY 0x44 // Battery Board

// Create an array of current sensors and their labels
Adafruit_INA219 ina219[] = {
    Adafruit_INA219(INA219_ADDR_SOLAR1),
    Adafruit_INA219(INA219_ADDR_SOLAR2),
    Adafruit_INA219(INA219_ADDR_SOLAR3),
    Adafruit_INA219(INA219_ADDR_SOLAR4),
    Adafruit_INA219(INA219_ADDR_BATTERY)};
const char *ina219Labels[] = {"Solar Panel 1", "Solar Panel 2", "Solar Panel 3", "Solar Panel 4", "Battery Board"};

// Create an instance of the Serial port for the GPS module
#define GPSSerial Serial7
Adafruit_GPS GPS(&GPSSerial);

// Pin assignments for the radio
const int CS_PIN = 38;
const int INT_PIN = 8;
const int SPI_MISO = 39;
const int SPI_MOSI = 26;
const int SPI_SCK = 27;

// Singleton instance of the radio driver
RH_RF22 rf23(CS_PIN, INT_PIN, hardware_spi1);

// Initialize the IMU objects
Adafruit_LIS3MDL lis = Adafruit_LIS3MDL();
Adafruit_LSM6DSOX lsm6dsox = Adafruit_LSM6DSOX();

// Define the analog input pins for the temperature sensors and their labels
const int sensorPins[] = {0, 1, 6, 7, 8, 9, 17};
const char *sensorLabels[] = {"OBC Board", "PDU Board", "Battery Board", "Solar Panel 1", "Solar Panel 2", "Solar Panel 3", "Solar Panel 4"};
const int numSensors = sizeof(sensorPins) / sizeof(sensorPins[0]);

// Raspberry Pi control
const int rpi_enable = 36;

void setup()
{
    // Initialize serial communication at 9600 bits per second
    Serial.begin(9600);
    while (!Serial)
    {
        delay(10);
    }

    // Start the GPS module at 9600 baud rate
    GPS.begin(9600);

    // Wait for 5 seconds to give time to open the serial monitor and the gps module time to boot
    delay(5000);

    // Clear any data from the GPS module
    while (GPS.available())
    {
        GPS.read();
    }

    // Assign SPI pins for the radio
    SPI1.setMISO(SPI_MISO);
    SPI1.setMOSI(SPI_MOSI);
    SPI1.setSCK(SPI_SCK);

    // Initialize Raspberry Pi control pin
    pinMode(rpi_enable, OUTPUT);
    digitalWrite(rpi_enable, LOW); // Make sure relay is off at startup

    // Toggle relay state after a delay
    delay(3000);                    // 3 seconds delay
    digitalWrite(rpi_enable, HIGH); // Turn on the relay
    Serial.print("Raspberry Pi has been powered on");
}

void loop()
{

    // GPS loop
    if (GPS.available() == 0)
    {
        Serial.println("GPS is not detected");
    }
    else
    {
        Serial.println("GPS is connected");
    }

    // Radio loop
    bool initSuccess = false;
    unsigned long startTime = millis();
    while (millis() - startTime < 5000)
    {
        if (rf23.init())
        {
            initSuccess = true;
            break;
        }
        else
        {
            delay(100);
        }
    }

    if (initSuccess)
    {
        Serial.println("Radio is connected");
    }
    else
    {
        Serial.println("Radio is not detected");
    }

    // IMU loop
    bool imu_connected = true;

    // Wait for 10 seconds for the IMU to connect and give data
    for (int i = 0; i < 100; i++)
    { // Repeat 100 times (i = 0 to 99)
        // Check if the IMU is giving data
        sensors_event_t accel, gyro, temp, mag;
        lsm6dsox.getEvent(&accel, &gyro, &temp); // Get accelerometer, gyroscope, and temperature data from LSM6DSOX
        lis.getEvent(&mag);                      // Get magnetometer data from LIS3MDL
        if (isnan(accel.acceleration.x) || isnan(gyro.gyro.x) || isnan(temp.temperature) || isnan(mag.magnetic.x))
        {
            // If any of the sensor readings is NaN (Not a Number), set the flag to false and exit the loop
            imu_connected = false;
            break;
        }
        delay(100); // Wait 100 milliseconds before checking the sensors again
    }

    // Print the result
    if (imu_connected)
    { // If the flag is true, the IMU is connected and giving data
        Serial.println("IMU is connected and reading data");
    }
    else
    { // If the flag is false, the IMU is not connected or not giving data
        Serial.println("IMU is is not detected or not reading data.");
    }

    // Current sensor loop
    for (int i = 0; i < 5; i++)
    {
        if (ina219[i].begin())
        {
            float current_mA = ina219[i].getCurrent_mA(); // Get the current reading in milliamps
            Serial.print(ina219Labels[i]);                // Print the label for the current sensor
            if (current_mA != 0)
            {
                Serial.println(" current sensor is connected and reading data.");
            }
            else
            {
                Serial.println(" current sensor is not reading data.");
            }
        }
        else
        {
            Serial.print(ina219Labels[i]); // Print the label for the current sensor
            Serial.println(" current sensor is not connected.");
        }
    }

    // Temperature sensor loop
    for (int i = 0; i < numSensors; i++)
    {
        int sensorValue = analogRead(sensorPins[i]); // Read the analog value from the sensor pin

        if (sensorValue > 15)
        { // this needs a threshold since we read an analog value even when no sensors are connected
            Serial.print(sensorLabels[i]);
            Serial.println(" temperature sensor is connected and reading data.");
        }
        else
        {
            Serial.print(sensorLabels[i]);
            Serial.println(" temperature sensor is not connected or not reading data.");
        }
    }

    Serial.println(); // Add a blank line between readings
    delay(10000);     // Wait for 10 seconds before taking the next reading
}
