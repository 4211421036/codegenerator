/***
   EEPROM Update method

   Stores values read from analog input 0 into the EEPROM.
   These values will stay in the EEPROM when the board is
   turned off and may be retrieved later by another sketch.

   If a value has not changed in the EEPROM, it is not overwritten
   which would reduce the life span of the EEPROM unnecessarily.
   
 ***/
 
#include <AT24Cxx.h>
#define i2c_address 0x50

/** the current address in the EEPROM (i.e. which byte we're going to write to next) **/
int address = 0;
AT24Cxx eep(i2c_address, 32); // Initilaize using AT24CXX(i2c_address, size of eeprom in KB).

void setup() {
	
	// initialize serial and wait for port to open:
	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}


}

void loop() {
  /***
    need to divide by 4 because analog inputs range from
    0 to 1023 and each byte of the EEPROM can only hold a
    value from 0 to 255.
  ***/
  int val = analogRead(0) / 4;
  Serial.println(val);

  /***
    Update the particular EEPROM cell.
    these values will remain there when the board is
    turned off.
  ***/
  eep.update(address, val);

  /***
    Advance to the next address, when at the end restart at the beginning.
  ***/
  address = address + 1;
  if (address == eep.length()) {
    address = 0;
  }

  /***
    As the EEPROM sizes are powers of two, wrapping (preventing overflow) of an
    EEPROM address is also doable by a bitwise and of the length - 1.

    ++address &= eep.length() - 1;
  ***/

  delay(100);
}
