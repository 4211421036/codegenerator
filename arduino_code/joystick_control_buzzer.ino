/*
 joystick_control_buzzer

 Simple sketch that show how to use the joystick of the DFRobot 
 Accessory Shield to control the active buzzer on the Accessory Shield
 
 created 3 Sept 2016
 by Biagio Montaruli
 updated 17 August 2017

 this code is in the public domain
 */
#include <AccessoryShield.h>

// create two new variables to store the previous joystick and
// the new joystick value
unsigned int joystickOldValue, joystickNewValue;
// declare new variables to store the initial frequency and buzzer
// delay used to play the buzzer
int frequency = 100, buzzerDelay = 1;
// minimum value of the buzzer frequency
const int minFrequency = 9;
// minimum value of buzzer delay
const int minBuzzerDelay = 0;

void setup() {
  // Initialize Serial comunication
  Serial.begin(115200);
  // wait for the serial port to connect, Needed for USB native only
  while(!Serial) ;
  // initialize the Accessory Shield library
  accessoryShield.begin();
  // store the initial value of the joystick
  joystickOldValue = accessoryShield.getJoystickValue(); 
}

void loop() {
  // get the new joystick value
  joystickNewValue = accessoryShield.getJoystickValue();

  // if the old value is different from the previous value ...
  if(joystickNewValue != joystickOldValue) {
    
    switch(joystickNewValue) {
      // if the joystick has been pushed, change the buzzer state
      case JOYSTICK_PUSH :
        Serial.println("Joystick pushed");
        if(accessoryShield.getBuzzerState() == LOW) {
          Serial.println("Start playing the buzzer");
          accessoryShield.playBuzzer(frequency, buzzerDelay);
        }
        else if(accessoryShield.getBuzzerState() == HIGH) {
          Serial.println("Turning OFF the buzzer...");
          accessoryShield.buzzerOFF();
        }
        break;
       // if the joystick has been moved right, increment the buzzer frequency
       case JOYSTICK_RIGHT :
        Serial.println("Joystick moved right");
        Serial.println("Incrementing the buzzer frequency");
        frequency++;
        Serial.print("Buzzer frequency = ");
        Serial.println(frequency);
        break;
      // if the joystick has been moved left, decrement the buzzer frequency
      case JOYSTICK_LEFT :
        Serial.println("Joystick moved left");
        Serial.println("Decrementing the buzzer frequency");
        frequency--;
        Serial.print("Buzzer frequency = ");
        Serial.println(frequency);
        if(frequency == minFrequency) {
          Serial.println("Resetting the buzzer frequency to the minimum value (10)");
          frequency = 10;
        }
        break;
      // if the joystick has been moved up, increment the buzzer delay
      case JOYSTICK_UP :
        Serial.println("Joystick moved up");
        Serial.println("Incrementing the buzzer delay");
        buzzerDelay++;
        Serial.print("Buzzer delay = ");
        Serial.println(buzzerDelay);
        break;
      #if !(defined(_VARIANT_ARDUINO_101_X_) || defined(__SAM3X8E__) || defined(ARDUINO_ARCH_SAMD))
      // if the joystick has been moved down, decrement the buzzer delay
      case JOYSTICK_DOWN :
        Serial.println("Joystick moved down");
        Serial.println("Decrementing the buzzer delay");
        buzzerDelay--;
        Serial.print("Buzzer delay = ");
        Serial.println(buzzerDelay);
        if(buzzerDelay == minBuzzerDelay) {
          Serial.println("Resetting the buzzer delay to the minimum value (1)");
          buzzerDelay = 1;
        }
        break;
      #endif
    }
    Serial.println();
    // update the old joystick value
    joystickOldValue = joystickNewValue;
  }
  delay(20);
}
