/**
 * @file Scan.ino
 * @brief Scan I2C Devices and identify as KeiganMotor
 * @details If an I2C device is found, check whether it is KeiganMotor by function KeiganMotor::readDeviceName
 * @note This sample is based on i2c_scanner from http://playground.arduino.cc/Main/I2cScanner
 * @date 2020/5/1
 * @author Takashi Tokuda (Keigan Inc.)
 */

#include "KM1_I2C.h"
#include <Wire.h>
//#include <M5Stack.h> // Please comment in if using M5Stack
 
void setup()
{
  Wire.begin();
 
  Serial.begin(115200);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2C Scanner");
}
 
 
void loop()
{
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
#ifdef _M5STACK_H_
    if(address == 0x75) return; // This avoids hang-up because M5Stack is equipped with IP5306 IC. If you cannot over-write a program, please long-press power switch before compile and release it after a few seconds from starting write. 
#endif
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
     
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      // Request Device name to identify KeiganMotor
      KeiganMotor motor(address); 
      char name[20] = {0};
      if(motor.readDeviceName(name)){
        Serial.print("This is a KeiganMotor: ");
        Serial.println(name);
      } else {
        Serial.println("An other I2C device");
      }
      
      
      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
 
  delay(3000);           // wait 3 seconds for next scan
}
