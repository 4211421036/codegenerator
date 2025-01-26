#include <CanSatKit.h>

using namespace CanSatKit;

// Remember to set the same radio parameters in
// transmitter and receiver boards!
Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,
            Bandwidth_125000_Hz,
            SpreadingFactor_9,
            CodingRate_4_8);

void setup() {
  radio.begin();
}

void loop() {
  radio.transmit("Hello World!");
  delay(250);
}
