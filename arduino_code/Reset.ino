/**
   @file Reset.ino
   @brief Reset and save all the registers.
   @date 2020/5/1
   @author Takashi Tokuda (Keigan Inc.)
*/

#include "KM1_I2C.h"

KeiganMotor motor(0x20); // Start I2C communication

void setup() {

  motor.resetAllRegisters();
  motor.saveAllRegisters();
  delay(2000);
  motor.reboot();

}

void loop() {

}
