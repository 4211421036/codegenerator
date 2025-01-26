/**
 * @file Dual.ino
 * @brief Run two KeiganMotors at the same time.
 * @details  See Scan.ino sample to know the current I2C address of KeiganMotor @n
 * See ChangeI2CAddress.ino sample to change I2C address
 * @note Manage I2C address of your KeiganMotors. Default I2C Slave Address is 0x20 (KeiganMotor KM-1 Series)
 * @date 2020/5/1
 * @author Takashi Tokuda (Keigan Inc.)
 */

#include "KM1_I2C.h"


KeiganMotor m1(0x20); 
KeiganMotor m2(0x30); 

void setup() {

  m1.led(1, 255, 0, 0); // Set LED color solid:red
  m2.led(1, 0, 255, 0); // Set LED color solid:green
  
  m1.enable(); // Enable Motor Action
  m2.enable();
  m1.speedRpm(10);
  m2.speedRpm(10);


}

void loop() {

  m1.moveByDistanceDegree(30);
  m2.moveByDistanceDegree(-30);
    
  delay(3000);

  m1.moveByDistanceDegree(-30);
  m2.moveByDistanceDegree(30);
  
  delay(3000);

}
