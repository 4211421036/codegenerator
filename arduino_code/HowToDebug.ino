/**
   @file HowToDebug.ino
   @brief Debug tips to handle write command and readStatus response from KeiganMotor
   @details This sample is useful for debug and development
   @date 2020/5/1
   @author Takashi Tokuda (Keigan Inc.)
*/

#include "KM1_I2C.h"


KeiganMotor motor(0x20); // Start I2C communication

void setup() {

  Serial.begin(115200);
  Serial.println("How to debug");
  Serial.println();

  bool success;

  success = motor.enableCheckSum(true, true);

  if(success) Serial.println("Success: [enableCheckSum]");
  else {
    Serial.println("Error: [enableCheckSum]");
    error_t error = motor.getError();
    Serial.print("error code: ");
    Serial.println(error.code);
  }  
  
  success = motor.led(1, 255, 0, 0, true); // Set LED color solid:red

  if(success) Serial.println("Success: [led]");
  else {
    Serial.println("Error: [led]");
    error_t error = motor.getError();
    Serial.print("error code: ");
    Serial.println(error.code);
  }
  
  success = motor.enable(true); // Enable Motor Action
 
  if(success) Serial.println("Success: [enable]");
  else {
    Serial.println("Error: [enable]");
    error_t error = motor.getError();
    Serial.print("error code: ");
    Serial.println(error.code);
  }  
  
  success = motor.runAtVelocityRpm(10, true); // Run at velocity [rpm]
  
  if(success) Serial.println("Success: [runAtVelocityRpm]");
  else {
    Serial.println("Error: [runAtVelocityRpm]");
    error_t error = motor.getError();
    Serial.print("error code: ");
    Serial.println(error.code);
  }    
  
  status_t status = motor.readStatus();
  
  if(status.isValid){
    Serial.println("Success: [readStatus]");
    Serial.print("isEnabled: ");
    Serial.println(status.isEnabled); // Motor is enabled or disabled
    Serial.print("isQueuePaused: ");
    Serial.println(status.isQueuePaused); // false if command queue is paused
    Serial.print("isMotorMeasNotifyEnabled: ");
    Serial.println(status.isMotorMeasNotifyEnabled); // always false (0) when using I2C 
    Serial.print("isIMUMeasNotifyEnabled: ");
    Serial.println(status.isIMUMeasNotifyEnabled); // always false (0) when using I2C        
    Serial.print("flashState: ");
    Serial.println(status.flashState); // flash state
    Serial.print("motorControlMode: ");
    Serial.println(status.motorControlMode); // motor control mode  

  } else {
    Serial.println("Invalid status.");
  }

}



void loop() {

}
