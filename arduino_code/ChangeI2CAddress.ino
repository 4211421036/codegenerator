/**
 * @file ChangeI2CAddress.ino
 * @brief Change I2C Slave Address from 0x20 to 0x30 @n
 * See Scan.ino sample to know the current I2C address of KeiganMotor
 * @note Default I2C Slave Address is 0x20 (KeiganMotor KM-1 Series)
 * @date 2020/5/1
 * @author Takashi Tokuda (Keigan Inc.)
 */

#include "KM1_I2C.h"

KeiganMotor motor(0x20);

void setup() {
  
  motor.i2cSlaveAddress(0x30); // Change I2C Address from 0x20 to 0x30
  motor.saveAllRegisters();
  delay(2000);
  motor.reboot(); 
    
}

void loop() {

}
