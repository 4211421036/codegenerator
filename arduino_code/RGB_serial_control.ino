/*
  RGB_serial_control
  
  Use the Serial comunication and the Serial Monitor to control the
  state of the RGB led of DFRobot Accessory Shield

  created 2 Sept 2016
  by Biagio Montaruli

 this code is in the public domain

  This example code is in the public domain.
 */
#include <AccessoryShield.h>

#define SERIAL_BAUDRATE 9600
 
void setup() {
  // initialize Serial comunication
  Serial.begin(SERIAL_BAUDRATE);
  // wait for the serial port to connect. Needed for Native USB only
  while(!Serial) { ; }
  Serial.print("Serial port initializated with baudrate ");
  Serial.println(SERIAL_BAUDRATE);
  // print instructions in the Serial Monitor
  Serial.println("Digit 'r' or 'R' to turn the red led ON");
  Serial.println("Digit 'g' or 'G' to turn the green led ON");
  Serial.println("Digit 'b' or 'B' to turn the blue led ON");

  // initialize the Accessory Shield library
  accessoryShield.begin();
}

void loop() {
  // while there are incoming bytes (characters) ...
  while(Serial.available() > 0) {
    Serial.println("Reading incoming byte from Serial port ...");
    // read the next byte (character)
    char led = Serial.read();
    Serial.println("Data received :");
    // print the byte as an ASCII character
    Serial.print(led);
    Serial.println(" (CHAR)");
    // print the decimal value of the ASCII chracter
    Serial.print(led, DEC);
    Serial.println(" (ASCII code)");
    // if you have typed 'r' or 'R', turn only the RED led ON
    if( (led == 'r') || (led == 'R') ) {
      Serial.println("Turning the red led ON ...");
      accessoryShield.setRGB(LED_ON, LED_OFF, LED_OFF);
    }
    // else if you have typed 'g' or 'G', turn only the GREEN led ON
    else if( (led == 'g') || (led == 'G') ) {
      Serial.println("Turning the green led ON ...");
      accessoryShield.setRGB(LED_OFF, LED_ON, LED_OFF);
    }
    // else if you have typed 'b' or 'B', turn only the BLUE led ON
    else if( (led == 'b') || (led == 'B') ) {
      Serial.println("Turning the blue led ON ...");
      accessoryShield.setRGB(LED_OFF, LED_OFF, LED_ON);
    }
    Serial.println();
  }
}
