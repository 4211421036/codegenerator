/*This code is for monitoring and logging data from the sesnors on the Artemis Cubesat,
including a Adafruit Mini GPS PA1010D module, an Adafruit LSM6DSOX + LIS3MDL Inertial Measurement Unit (IMU),
TMP36FSZ temperature sensors, and INA219 current/voltage sensors.
The data is collected and displayed through the serial monitor. */

#include <Arduino.h>
#include <Adafruit_GPS.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

// GPS
#define GPSSerial Serial7
Adafruit_GPS GPS(&GPSSerial);
const bool GPSECHO = true;
unsigned long timer = millis();
bool gpsConnected = false;

// IMU
Adafruit_LIS3MDL lis = Adafruit_LIS3MDL();
Adafruit_LSM6DSOX lsm6dsox = Adafruit_LSM6DSOX();
bool imu_found = false;

// Temperature Sensors
const int NUM_SENSORS = 7;
const int TEMP_SENSOR_PINS[NUM_SENSORS] = {A0, A1, A6, A7, A8, A9, A17};
const char *SENSOR_LABELS[NUM_SENSORS] = {"OBC", "PDU", "Battery Board", "Solar Panel 1", "Solar Panel 2", "Solar Panel 3", "Solar Panel 4"};
const float MV_PER_DEGREE_F = 1.0;
const float OFFSET_F = 58.0;
const float MV_PER_ADC_UNIT = 3300.0 / 1024.0;

// INA219
#define INA219_ADDR_SOLAR1 0x40
#define INA219_ADDR_SOLAR2 0x41
#define INA219_ADDR_SOLAR3 0x42
#define INA219_ADDR_SOLAR4 0x43
#define INA219_ADDR_BATTERY 0x44
Adafruit_INA219 ina219s[] = {
    Adafruit_INA219(INA219_ADDR_SOLAR1),
    Adafruit_INA219(INA219_ADDR_SOLAR2),
    Adafruit_INA219(INA219_ADDR_SOLAR3),
    Adafruit_INA219(INA219_ADDR_SOLAR4),
    Adafruit_INA219(INA219_ADDR_BATTERY)};
const char *ina219Labels[] = {"Solar Panel 1", "Solar Panel 2", "Solar Panel 3", "Solar Panel 4", "Battery Board"};

float readTemperatureCelsius(int sensorPin)
{
    int adcValue = analogRead(sensorPin);
    float voltage = adcValue * MV_PER_ADC_UNIT;
    float temperatureF = (voltage - OFFSET_F) / MV_PER_DEGREE_F;
    float temperatureC = (temperatureF - 32) * 5 / 9;
    return temperatureC;
}

void setup()
{
    // GPS Setup
    Serial.begin(115200);
    GPS.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    GPS.sendCommand(PGCMD_ANTENNA);
    delay(10000);
    if (GPS.available() == 0)
    {
        Serial.println("GPS module not detected");
        gpsConnected = false;
    }
    else
    {
        Serial.println("GPS module connected");
        gpsConnected = true;
    }
    while (GPS.available())
    {
        GPS.read();
    }

    // IMU Setup
    Serial.begin(9600);
    while (!Serial)
    {
        delay(5000);
    }
    if (!lis.begin_I2C() || !lsm6dsox.begin_I2C())
    {
        Serial.println("IMU not detected");
    }
    else
    {
        Serial.println("IMU connected");
        imu_found = true;
    }

    // Temperature Sensors Setup
    for (int i = 0; i < NUM_SENSORS; i++)
    {
        pinMode(TEMP_SENSOR_PINS[i], INPUT);
    }

    // INA219 Setup
    for (int i = 0; i < 5; i++)
    {
        bool sensorConnected = ina219s[i].begin(&Wire2);
        if (sensorConnected)
        {
            Serial.print(ina219Labels[i]);
            Serial.println(" current sensor is connected");
        }
        else
        {
            Serial.print(ina219Labels[i]);
            Serial.println(" current sensor is not detected");
        }
        ina219s[i].setCalibration_16V_400mA();
    }
}

void loop()
{
    // GPS Loop
    if (gpsConnected)
    {
        char c = GPS.read();
        if (GPSECHO)
        {
            if (c)
                Serial.print(c);
        }
        if (GPS.newNMEAreceived())
        {
            if (!GPS.parse(GPS.lastNMEA()))
            {
                return;
            }
        }
        if (timer > millis())
            timer = millis();
        if (millis() - timer > 5000)
        {
            timer = millis();
            Serial.print("\nTime: ");
            Serial.print(GPS.hour, DEC);
            Serial.print(':');
            Serial.print(GPS.minute, DEC);
            Serial.print(':');
            Serial.print(GPS.seconds, DEC);
            Serial.print('.');
            Serial.println(GPS.milliseconds);
            Serial.print("Date: ");
            Serial.print(GPS.day, DEC);
            Serial.print('/');
            Serial.print(GPS.month, DEC);
            Serial.print("/20");
            Serial.println(GPS.year, DEC);
            Serial.print("Fix: ");
            Serial.print((int)GPS.fix);
            Serial.print(" quality: ");
            Serial.println((int)GPS.fixquality);
            if (GPS.fix)
            {
                Serial.print("Location: ");
                Serial.print(GPS.latitudeDegrees,
                             4);
                Serial.print(", ");
                Serial.println(GPS.longitudeDegrees, 4);
                Serial.print("Speed (knots): ");
                Serial.println(GPS.speed);
                Serial.print("Angle: ");
                Serial.println(GPS.angle);
                Serial.print("Altitude: ");
                Serial.println(GPS.altitude);
                Serial.print("Satellites: ");
                Serial.println((int)GPS.satellites);
                Serial.println();
            }
        }
    }

    // IMU Loop
    if (imu_found)
    {
        sensors_event_t accel, gyro, temp, mag;
        lsm6dsox.getEvent(&accel, &gyro, &temp);
        lis.getEvent(&mag);

        Serial.print("Acceleration (m/s^2): x = ");
        Serial.print(accel.acceleration.x);
        Serial.print(", y = ");
        Serial.print(accel.acceleration.y);
        Serial.print(", z = ");
        Serial.println(accel.acceleration.z);

        Serial.print("Gyro (dps): x = ");
        Serial.print(gyro.gyro.x);
        Serial.print(", y = ");
        Serial.print(gyro.gyro.y);
        Serial.print(", z = ");
        Serial.println(gyro.gyro.z);

        Serial.print("IMU Temperature (°C): ");
        Serial.println(temp.temperature);

        Serial.print("Magnetic field (uT): x = ");
        Serial.print(mag.magnetic.x);
        Serial.print(", y = ");
        Serial.print(mag.magnetic.y);
        Serial.print(", z = ");
        Serial.println(mag.magnetic.z);
        Serial.println();
        delay(10000);
    }

    // Temperature Sensors Loop
    for (int i = 0; i < NUM_SENSORS; i++)
    {
        float temperatureC = readTemperatureCelsius(TEMP_SENSOR_PINS[i]);
        Serial.print(SENSOR_LABELS[i]);
        Serial.print(" Temperature: ");
        Serial.print(temperatureC);
        Serial.println(" °C");
    }
    Serial.println();
    delay(5000);

    // INA219 Loop
    for (int i = 0; i < 5; i++)
    {
        float current_mA = ina219s[i].getCurrent_mA();
        float bus_voltage_V = ina219s[i].getBusVoltage_V();

        Serial.print(ina219Labels[i]);
        Serial.print(" current: ");
        Serial.print(current_mA);
        Serial.print(" mA");
        Serial.print(", bus voltage: ");
        Serial.print(bus_voltage_V);
        Serial.println(" V");
    }
    Serial.println();
    delay(1000);
}
