/**
 * @file MoveBy.ino
 * @brief Move by distance (move to relative position) example. (Position control).
 * @note Default I2C Slave Address is 0x20 (KeiganMotor KM-1 Series)
 * @date 2020/5/1
 * @author Takashi Tokuda (Keigan Inc.)
 */

#include "KM1_I2C.h"

KeiganMotor motor(0x20); // Start I2C communication

void setup() {

  motor.led(1, 255, 0, 0); // Set LED color solid:red
  motor.enable(); // Enable Motor Action
  motor.acc(100);
  motor.dec(100);
  motor.speedRpm(100);
 
  for (int i = 0; i<12; i++){
    motor.moveByDistanceDegree(30);
    delay(2000);
  }

  motor.led(1, 0, 255, 0); // Set LED color solid:green

}

void loop() {
  

}