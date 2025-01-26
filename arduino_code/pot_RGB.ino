/*
 pot_RGB

 Simple sketch that show how to use the potentiometer of the 
 DFRobot Accessory Shield to control the RGB led on
 
 created 4 Sept 2016
 by Biagio Montaruli

 this code is in the public domain
 */
#include <AccessoryShield.h>

void setup() {
  // initialize the Accessory Shield library
  accessoryShield.begin();
}

void loop() {
  // read the pot trimmer value and use it to set the delay time
  // used to change the RGB led state
  int delayTime = accessoryShield.readPot();
  
  // turn only the RED led ON (GREEN and BLUE leds are OFF) 
  accessoryShield.setRGB(LED_ON, LED_OFF, LED_OFF);
  // wait for a delay time specified by the pot value
  delay(delayTime);
  // turn only the GREEN led ON (RED and BLUE leds are OFF)
  accessoryShield.setRGB(LED_OFF, LED_ON, LED_OFF);
  // wait for a delay time specified by the pot value
  delay(delayTime);
  // turn only the BLUE led ON (RED and GREEN leds are OFF)
  accessoryShield.setRGB(LED_OFF, LED_OFF, LED_ON);
  // wait for a delay time specified by the pot value
  delay(delayTime);
  // turn the RED and GREEN leds ON -> you get the YELLOW color
  accessoryShield.setRGB(LED_ON, LED_ON, LED_OFF);
  // wait for a delay time specified by the pot value
  delay(delayTime);
  // turn the GREEN and BLUE leds ON -> you get the CYANO color
  accessoryShield.setRGB(LED_OFF, LED_ON, LED_ON);
  // wait for a delay time specified by the pot value
  delay(delayTime);
  // turn the RED and BLUE leds ON -> you get the MAGENTA color
  accessoryShield.setRGB(LED_ON, LED_OFF, LED_ON);
  // wait for a delay time specified by the pot value
  delay(delayTime);
  // turn the RED, GREEN and BLUE leds ON -> you get the WHITE color
  accessoryShield.setRGB(LED_ON, LED_ON, LED_ON);
  // wait for a delay time specified by the pot value
  delay(delayTime);
}
