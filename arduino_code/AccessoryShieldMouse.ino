/*
 AccessoryShieldMouse

 Controls the mouse using an Arduino/Genuino Due, Zero or an
 ATMEGA32U4 boards with the joystick on DFRobot Accessory Shield

 The mouse movement is always relative. This sketch reads
 four pushbuttons, and uses them to set the movement of the mouse.

 WARNING:  When you use the Mouse.move() command, the Arduino takes
 over your mouse!  Make sure you have control before you use the mouse commands.

 created 3 Sept 2016
 by Biagio Montaruli
 updated 17 August 2017

 this code is in the public domain

 */
#include <AccessoryShield.h>
#include <Mouse.h>

// Uncomment the next line for sketch debugging
//#define SKETCH_DEBUG

// offset for mouse X and Y movements
unsigned int xDistance, yDistance;
// maximum and minimum value for the X and Y distance offset
const unsigned int maxDistanceOffset = 20;
const unsigned int minDistanceOffset = 1;
// response delay of the mouse, in ms
const int responseDelay = 10;

void setup() {
  // initialize the Accessory Shield library
  accessoryShield.begin();
  // initialize mouse control:
  Mouse.begin();
  
#ifdef SKETCH_DEBUG
  Serial.begin(9600);
  while(!Serial) ;
#endif
}

void loop() {
  // read a new value from th joystick
  JoystickMode joystickValue = accessoryShield.getJoystickValue();
  
  // set the distance offset using the pot trimmer of Accessory Shield
  unsigned int distanceOffset = accessoryShield.readPot();
  // calibrate and map the pot value 
  distanceOffset = map(distanceOffset, 0, 1023, minDistanceOffset, maxDistanceOffset);
  // set the X and Y distance :
  // xDistance and yDistance are used to set how the mouse sould be moved
  // in orizontal (X axis) or in vertical (Y axis) 
  xDistance = yDistance = distanceOffset;

  // check if the joystick has been moved
#if (defined(_VARIANT_ARDUINO_101_X_) || defined(__SAM3X8E__) || defined(ARDUINO_ARCH_SAMD))
  if(joystickValue != JOYSTICK_NONE_OR_DOWN) {
#else
  if(joystickValue != JOYSTICK_NONE) {
#endif
    // print values of X and Y distance
#ifdef SKETCH_DEBUG
    Serial.print("X distance offset= ");
    Serial.println(xDistance);
    Serial.print("Y distance offset= ");
    Serial.println(yDistance);
#endif
    
    switch(joystickValue) {
      // if the joystick has been pushed, click with mouse left button
      case JOYSTICK_PUSH :
        Mouse.click(MOUSE_LEFT);
        break;
      // if the joystick has been moved up, move up the mouse cursor
      case JOYSTICK_UP :
        Mouse.move(0, -yDistance, 0);
        break;
#if !(defined(_VARIANT_ARDUINO_101_X_) || defined(__SAM3X8E__) || defined(ARDUINO_ARCH_SAMD))
      // if the joystick has been moved down, move down the mouse cursor
      case JOYSTICK_DOWN :
        Mouse.move(0, yDistance, 0);
        break;
#endif
      // if the joystick has been moved right, move right the mouse cursor
      case JOYSTICK_RIGHT :
        Mouse.move(xDistance, 0, 0);
        break;
      // if the joystick has been moved left, move left the mouse cursor
      case JOYSTICK_LEFT :
        Mouse.move(-xDistance, 0, 0);
        break;
      
    }
  }
  // wait for a small delay time so the mouse doesn't move too fast:
  delay(responseDelay);
}
