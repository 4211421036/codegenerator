#include <Arduino.h>
#include "AT24CM01.h"

AT24CM01 EEPROM;                                                    

unsigned long test = 20304050UL;                // Data
size_t test_size = sizeof(unsigned long);       // Data size
unsigned long test_recieve = 0;                 // Data recieved

void setup() 
{
  Serial.begin(9600); 

  EEPROM.begin();                                               

  EEPROM.write(0, test);                              // write data to address 0
  test_recieve = EEPROM.read(0, test_size);           // read data from address 0, of ul size
  Serial.println(test_recieve);

  EEPROM.write(0 + test_size, test);                  // repeat, adding size to last address
  test_recieve = EEPROM.read(0, test_size);
  Serial.print(test_recieve);
}

void loop() {}
