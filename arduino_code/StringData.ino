#include <Arduino.h>
#include "AT24CM01.h"

AT24CM01 EEPROM;            // Library 

#define AR_LEN 6            // Array length
char textarray[AR_LEN];     // text buffer
String text = "Hello";      // string to be stored

void setup() 
{
  Serial.begin(9600);

  EEPROM.begin();

  Serial.print("\n.................... Recalling String .................... \n");
  Serial.print("\n Writing << "); Serial.print(text);
  text.toCharArray(textarray, (AR_LEN));               // Use arduino to move String to text array
  EEPROM.writeCharArray(20, textarray, AR_LEN-1);      // EEPROM.writeArrayType(address, array, arraysize)  
  
  memset(textarray, 0, AR_LEN-1);                      // Clear text array
  text = "";                                           // Clear String

  EEPROM.readCharArray(20, textarray, AR_LEN-1);       // ReadArrayType(address, textarraybuffer, textsize)
  Serial.print("\n Recieved >> ");
  for(uint8_t x = 0; x < AR_LEN-1; x++)
    text += textarray[x];                              // Add each array value to the string.

  Serial.print(text);                                  // Print string
}


void loop(){}
