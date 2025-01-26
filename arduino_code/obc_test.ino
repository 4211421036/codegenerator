/*This code performs a check of the components located on the OBC and reports their status.*/

#include <Arduino.h>
#include <Adafruit_GPS.h>
#include <RH_RF22.h>
#include <RHHardwareSPI1.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_Sensor.h>

// Relay control
const int rpi_enable = 36;

// GPS module
#define GPSSerial Serial7
Adafruit_GPS GPS(&GPSSerial);

// Radio module
const int CS_PIN = 38;
const int INT_PIN = 8;
const int SPI_MISO = 39;
const int SPI_MOSI = 26;
const int SPI_SCK = 27;
RH_RF22 rf23(CS_PIN, INT_PIN, hardware_spi1);

// IMU modules
Adafruit_LIS3MDL lis = Adafruit_LIS3MDL();
Adafruit_LSM6DSOX lsm6dsox = Adafruit_LSM6DSOX();

// Temperature sensor
const int sensorPin = 0;
const char *sensorLabel = "Temperature Sensor";

void setup()
{
    pinMode(rpi_enable, OUTPUT);
    digitalWrite(rpi_enable, LOW);

    Serial.begin(115200);

    delay(3000);
    digitalWrite(rpi_enable, HIGH);
    Serial.println("RPi relay is ON");

    // GPS setup
    GPS.begin(9600);
    delay(5000);
    while (GPS.available())
    {
        GPS.read();
    }

    // Radio setup
    SPI1.setMISO(SPI_MISO);
    SPI1.setMOSI(SPI_MOSI);
    SPI1.setSCK(SPI_SCK);

    // IMU setup
    while (!Serial)
    {
        delay(10);
    }
    if (!lis.begin_I2C() || !lsm6dsox.begin_I2C())
    {
        Serial.println("Failed to find LSM6DSOX and/or LIS3MDL");
        while (1)
        {
            delay(10);
        }
    }
}

void loop()
{
    // GPS check
    if (GPS.available() == 0)
    {
        Serial.println("GPS not detected");
    }
    else
    {
        Serial.println("GPS connected");
    }
    delay(1000);

    // Radio check
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
        Serial.println("Radio not connected");
    }
    delay(2000);

    // IMU check
    bool imu_connected = true;
    for (int i = 0; i < 100; i++)
    {
        sensors_event_t accel, gyro, temp, mag;
        lsm6dsox.getEvent(&accel, &gyro, &temp);
        lis.getEvent(&mag);
        if (isnan(accel.acceleration.x) || isnan(gyro.gyro.x) || isnan(temp.temperature) || isnan(mag.magnetic.x))
        {
            imu_connected = false;
            break;
        }
        delay(100);
    }
    if (imu_connected)
    {
        Serial.println("IMU is connected and reading data");
    }
    else
    {
        Serial.println("IMU is not connected or not reading data");
    }
    delay(10000);

    // Temperature sensor check
    int sensorValue = analogRead(sensorPin);
    if (sensorValue > 15)
    {
        Serial.print(sensorLabel);
        Serial.println(" is connected and reading data.");
    }
    else
    {
        Serial.print(sensorLabel);
        Serial.println(" is not connected or not reading data.");
    }
    Serial.println(); // Add a blank line between readings
    delay(10000);     // Wait for 10 seconds before taking the next reading
}