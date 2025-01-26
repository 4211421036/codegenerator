/**
 * @file MoveTo.ino
 * @brief Move to absolute position example. (Position control).
 * @note Default I2C Slave Address is 0x20 (KeiganMotor KM-1 Series)
 * @date 2020/5/1
 * @author Takashi Tokuda (Keigan Inc.)
 */

#include "KM1_I2C.h"

KeiganMotor motor(0x20); // Start I2C communication

void setup() {

  motor.led(1, 255, 0, 0); // Set LED color solid:red
  motor.speedRpm(10); // Set speed
  //motor.presetPosition(0); // Set the current position as zero
  motor.enable(); // Enable Motor Action
  motor.moveToPositionDegree(90); // Move to position 90 degree
  motor.wait(5000); // Wait for 3000 msec until next command
  motor.led(1, 0, 255, 0); // Set LED color solid:green
  motor.moveToPositionDegree(0); // Move to position 0 
  
}

void loop() {

}
