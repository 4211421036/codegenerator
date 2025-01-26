/**
 * @file Run.ino
 * @brief Run example (Velocity control).
 * @note Default I2C Slave Address is 0x20 (KeiganMotor KM-1 Series)
 * @date 2020/5/1
 * @author Takashi Tokuda (Keigan Inc.)
 */

#include "KM1_I2C.h"

KeiganMotor motor(0x20); // Start I2C communication

void setup() {

  motor.enable(); // Enable Motor Action
  motor.speedRpm(10); // Set Speed
  
  motor.led(1, 255, 0, 0); // Set LED color solid:red
  motor.runForward(); // Run forward
  
  delay(5000);
  
  motor.led(1, 0, 255, 0); // Set LED color solid:green
  motor.runReverse(); // Run reverse

  delay(5000);
    
  motor.led(1, 0, 0, 255); // Set LED color solid:blue
  motor.stop(); // Stop

  /**
   * @note runAtVelocity(rpm) is available instead of speedRpm() and runForward() or runReverse().
   */
  
}

void loop() {

}
