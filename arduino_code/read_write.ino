#include <AT24C256.h>

AT24C256 eeprom(0x50);

void setup() {
 
  char message[30];
  char writemessage[] = "Hello World!";
  
  Serial.begin(9600);
  Serial.println("reading eeprom");
  eeprom.read(0, (uint8_t*) message, sizeof(message));
  Serial.println(message);

  eeprom.write(0, (uint8_t*)writemessage, sizeof(message));
  Serial.println("wrote eeprom");
  
  Serial.println("reading eeprom");
  eeprom.read(0, (uint8_t*) message, sizeof(message));
  Serial.println(message);
}

void loop() {
  // put your main code here, to run repeatedly:

}
