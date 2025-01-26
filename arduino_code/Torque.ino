/**
   @file Torque.ino
   @brief Max Torque and position control example
   @details Increase max torque during position control.
   @date 2020/5/1
   @author Takashi Tokuda (Keigan Inc.)
   @note Please adjust TORQUE_INCREMENT_VAL responding to your motor torque
*/

#include "KM1_I2C.h"

/**
 * @def TORQUE_INCREMENT_VAL
 * @brief Increase maxTorque by every 2 seconds
 */
#define TORQUE_INCREMENT_VAL (0.03)

KeiganMotor motor(0x20); // Start I2C communication

void setup() {

  Serial.begin(115200);
  Serial.println("Torque control ");
  Serial.println();
  Serial.println("Please touch motor !!!");
  Serial.println();

  delay(2000);
  
  float maxTrq = 0;

  maxTrq = motor.readMaxTorque();
  Serial.print("Current max torque: ");
  Serial.print(maxTrq);
  Serial.println(" [N*m]");
  Serial.println();

  float diff = maxTrq * 0.1;

  motor.led(1, 255, 0, 0); // Set LED color solid:red

  // Position control
  motor.enable(); // Enable Motor Action
  motor.speedRpm(100); // Set speed [rpm]
  motor.moveByDistance(0); // Fix at the current position

  int count = 0;

  for (int i = 0; i < 10; i++) {
    motor.maxTorque(TORQUE_INCREMENT_VAL * i); // Increase maxTorque by every 2 seconds
    Serial.print("Increase max torque to: ");
    Serial.print(motor.readMaxTorque());
    Serial.println(" [N*m]");
    Serial.println();
    delay(2000);
  }

  delay(5000);
  motor.maxTorque(maxTrq); // Resume maxTorque

  Serial.print("Current max torque: ");
  Serial.print(motor.readMaxTorque());
  Serial.println(" [N*m]");
  Serial.println();


}



void loop() {

}
