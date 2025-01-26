/*
 pot_RGB_random_brightness

 Simple sketch that show how to use the potentiometer of the 
 DFRobot Accessory Shield to control the blinking of RGB leds and
 use the random() and randomSeed() functions to generate random 
 numbers to set RGB led brightness
 
 created 4 Sept 2016
 by Biagio Montaruli

 this code is in the public domain
 */
#include <AccessoryShield.h>

void setup() {
  // initialize Serial comunication
  Serial.begin(9600);
  // wait for the serial port to connect, Needed for USB native only
  while(!Serial) ;
  // initialize the Accessory Shield library
  accessoryShield.begin();
  // set seed for random number generator
  randomSeed(A2);
}

void loop() {
  // set delay time with the value read from the potentiometer
  int delayTime = accessoryShield.readPot();
  // declare an array to store the initial values of RGB led brightness 
  unsigned int RgbLight[3] = {1023, 1023, 1023};
  // compute the RGB led brightness using the random() function which
  // generates random numbers
  for(int i = 0; i < 3; i++) {
    // generate a random number from 768 to 1023 and subtract the
    // initial led brightness of the random generated value 
    RgbLight[i] -= random(768, 1023);
    Serial.print(getLedColor(i));
    Serial.print(" led brightness = ");
    Serial.println(RgbLight[i]);
  }
  Serial.println();
  // turn the RGB leds ON with the previously specified brightness
  accessoryShield.setRGB(RgbLight[0], RgbLight[1], RgbLight[2]);
  // wait for a delay time specified by the pot value
  delay(delayTime);
}

// function that receives the led number and returns a String
// object that represents the led color
String getLedColor(int ledNum) {
  String color = "Red";
  if(ledNum == 1) {
    color.replace("Red", "Green");
  }
  else if(ledNum == 2) {
    color.replace("Red", "Blue");
  }
  return color;
}
