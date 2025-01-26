/*
 Helloworld_RGB

 Simple sketch that show how to use Accessory Shield library to
 control the RGB led
 
 created 4 Sept 2016
 by Biagio Montaruli

 this code is in the public domain
 */
#include <AccessoryShield.h>

void setup() {
  // initialize the accessory Shield library
  accessoryShield.begin();
}

void loop() {
  // turn the RED led ON
  accessoryShield.redON();
  // wait for 1 second (1000 milliseconds)
  delay(1000);
  // turn the RED led OFF
  accessoryShield.redOFF();
  // wait for 1 second (1000 milliseconds)
  delay(1000);
  // turn the GREEN led ON
  accessoryShield.greenON();
  // wait for 1 second (1000 milliseconds)
  delay(1000);
  // turn the GREEN led OFF
  accessoryShield.greenOFF();
  // wait for 1 second (1000 milliseconds)
  delay(1000);
  // turn the BLUE led ON
  accessoryShield.blueON();
  // wait for 1 second (1000 milliseconds)
  delay(1000);
  // turn the BLUE led OFF
  accessoryShield.blueOFF();
  // wait for 1 second (1000 milliseconds)
  delay(1000);
}
