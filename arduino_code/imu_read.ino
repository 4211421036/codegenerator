/*This code reads data from the Adafruit's LIS3MDL magnetometer and LSM6DSOX IMU sensors.
It initializes the sensors, checks their presence, and continuously outputs acceleration, gyroscope, temperature,
and magnetic field data through the serial monitor.*/

#include <Adafruit_LIS3MDL.h>
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_Sensor.h>

// Initialize the LIS3MDL and LSM6DSOX objects
Adafruit_LIS3MDL lis = Adafruit_LIS3MDL();
Adafruit_LSM6DSOX lsm6dsox = Adafruit_LSM6DSOX();

bool imu_found = false;

void setup()
{
    // Start the serial communication
    Serial.begin(9600);
    // Wait for the serial monitor to open
    while (!Serial)
    {
        delay(5000);
    }

    // Check if the LSM6DSOX and LIS3MDL are found
    if (!lis.begin_I2C() || !lsm6dsox.begin_I2C())
    {
        // Print a warning message
        Serial.println("IMU not detected");
    }
    else
    {
        Serial.println("IMU connected");
        imu_found = true;
    }
}

void loop()
{
    if (imu_found)
    {
        // Create variables to store the sensor readings
        sensors_event_t accel, gyro, temp, mag;

        // Read the accelerometer, gyroscope, and temperature data from the LSM6DSOX
        lsm6dsox.getEvent(&accel, &gyro, &temp);
        // Read the magnetometer data from the LIS3MDL
        lis.getEvent(&mag);

        // Print the accelerometer readings
        Serial.print("Acceleration (m/s^2): x = ");
        Serial.print(accel.acceleration.x);
        Serial.print(", y = ");
        Serial.print(accel.acceleration.y);
        Serial.print(", z = ");
        Serial.println(accel.acceleration.z);

        // Print the gyroscope readings
        Serial.print("Gyro (dps): x = ");
        Serial.print(gyro.gyro.x);
        Serial.print(", y = ");
        Serial.print(gyro.gyro.y);
        Serial.print(", z = ");
        Serial.println(gyro.gyro.z);

        // Print the temperature reading
        Serial.print("Temperature (°C): ");
        Serial.println(temp.temperature);

        // Print the magnetometer readings
        Serial.print("Magnetic field (uT): x = ");
        Serial.print(mag.magnetic.x);
        Serial.print(", y = ");
        Serial.print(mag.magnetic.y);
        Serial.print(", z = ");
        Serial.println(mag.magnetic.z);

        // Wait for 10 seconds before reading the sensors again
        delay(10000);
    }
}
