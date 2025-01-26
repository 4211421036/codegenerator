/*
 joystick_control_relay

 Simple sketch that show how to use the joystick of the DFRobot 
 Accessory Shield to control the relay on the Accessory Shield
 
 created 3 Sept 2016
 by Biagio Montaruli
 updated 17 August 2017

 this code is in the public domain
 */
#include <AccessoryShield.h>

// macro used to print the relay state 
#define relayState accessoryShield.getRelayState() ? "ON" : "OFF"

// create two new variables to store the previous joystick and
// the new joystick value
unsigned int joystickOldValue, joystickNewValue;

void setup() {
  // initialize Serial comunication
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
  
  // if the old value is different from the previous value, check
  // the joystick state :
  // if the joystick has been moved RIGHT, print the joystick state
  // and activate the relay
  // else if if the joystick has been moved LEFT, print the joystick state
  // and disable the relay
  if(joystickNewValue != joystickOldValue) {
    if(joystickNewValue == JOYSTICK_RIGHT) {
      Serial.println("Joystick moved RIGHT");
      Serial.println("Turning the the relay ON...");
      accessoryShield.relayON();
      Serial.print("Relay is ");
      Serial.println(relayState);
    }
    else if(joystickNewValue == JOYSTICK_LEFT) {
      Serial.println("Joystick moved LEFT");
      Serial.println("Turning the relay OFF");
      accessoryShield.relayOFF();
      Serial.print("Relay is ");
      Serial.println(relayState);
    }
    Serial.println();
  }
  delay(60);
}
      
