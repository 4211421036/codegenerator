/*
 joystick

 Simple sketch that show how to use the joystick of the DFRobot 
 Accessory Shield to control the RGB led on the Accessory Shield
 
 created 4 Sept 2016
 by Biagio Montaruli

 this code is in the public domain
 */
#include <AccessoryShield.h>

// create two new variables to store the previous joystick and
// the new joystick value 
JoystickMode joystickOldValue, joystickNewValue;

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

  // if the new joystick value is different from the previous value ...
  if(joystickNewValue != joystickOldValue) {
    
    switch(joystickNewValue) {
      // if the joystick has been moved right, change the red led state
      case JOYSTICK_RIGHT :
        Serial.println("Joystick moved right");
        if( accessoryShield.getRGBstate(RED_LED) == LED_OFF ) {
          Serial.println("Turning the red led ON");
          accessoryShield.redON();
        }
        else if( accessoryShield.getRGBstate(RED_LED) == LED_ON ) {
          Serial.println("Turning the red led OFF");
          accessoryShield.redOFF();
        }
        break;
      // if the joystick has been moved up, change the green led state
      case JOYSTICK_UP :
        Serial.println("Joystick moved up");
        if( accessoryShield.getRGBstate(GREEN_LED) == LED_OFF ) {
          Serial.println("Turning the green led ON");
          accessoryShield.greenON();
        }
        else if( accessoryShield.getRGBstate(GREEN_LED) == LED_ON ) {
          Serial.println("Turning the green led OFF");
          accessoryShield.greenOFF();
        }
        break;
      // if the joystick has been moved left, change the blue led state
      case JOYSTICK_LEFT :
        Serial.println("Joystick moved left");
        if( accessoryShield.getRGBstate(BLUE_LED) == LED_OFF ) {
          Serial.println("Turning the blue led ON");
          accessoryShield.blueON();
        }
        else if( accessoryShield.getRGBstate(BLUE_LED) == LED_ON ) {
          Serial.println("Turning the blue led OFF");
          accessoryShield.blueOFF();
        }
        break;
      // if the joystick has been pushed, turn all the leds OFF
      case JOYSTICK_PUSH :
        Serial.println("Joystick pushed");
        Serial.println("Turning all leds OFF");
        accessoryShield.setRGB(LED_OFF, LED_OFF, LED_OFF);
        break;
      default :
        break;
    }
    // update the old joystick value
    joystickOldValue = joystickNewValue;
  }
  delay(20);
}
