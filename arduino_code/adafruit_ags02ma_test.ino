/* Note this sensor is very very picky about I2C speed - it wants maximum 20KHz which is way 
 *  below the standard 100KHz. Not all Arduino-compatible platforms successfully set the clock
 *  speed that low. We've tested this library with Atmega328 (Arduino UNO etc) and ESP32 sucessfully!
 */

#include "Adafruit_AGS02MA.h"

Adafruit_AGS02MA ags;

void setup() {
  Serial.begin(115200);
  Serial.println(F("Adafruit AGS20MA Test"));

  if (! ags.begin(&Wire, 0x1A)) {
  //if (! ags.begin(&Wire1, 0x1A)) { // or use Wire1 instead!
    Serial.println("Couldn't find AGS20MA sensor, check your wiring and pullup resistors!");
    while (1) yield();
  }

  if (ags.getFirmwareVersion() == 0) {
    Serial.println(F("Could not read firmware, I2C communications issue?"));
    while (1) yield();
  }

  Serial.print("Firmware version: 0x");
  Serial.println(ags.getFirmwareVersion(), HEX);
  ags.printSensorDetails();

  // uncomment to change address, will need to restart and update the begin() argument!
  //ags.setAddress(0x1A); while (1);
}

void loop() {
  uint32_t resistance = ags.getGasResistance();
  uint32_t tvoc = ags.getTVOC();
  
  if (resistance == 0) {
    Serial.println(F("Failure reading resistance, I2C communications issue?"));
  } else {
    float kohm = resistance / 1000.0;
    Serial.print(F("Gas resistance: "));
    Serial.print(kohm);
    Serial.println(" Kohms");
  }

  if (tvoc == 0) {
    Serial.println(F("Failure reading TVOC, I2C communications issue?"));
  } else {
    Serial.print(F("TVOC: "));
    Serial.print(tvoc);
    Serial.println(" ppb");
  }

  delay(1000);
}