#include <CanSatKit.h>
#include <SPI.h>
#include <SD.h>

using namespace CanSatKit;

int counter = 1;
bool led_state = false;
long last_blink;
File file;

Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,
            Bandwidth_500000_Hz,
            SpreadingFactor_7,
            CodingRate_4_8);

void setup() {
  while(!SerialUSB);
  SerialUSB.begin(115200);
  pinMode(Pins::LED, OUTPUT);

  if (!SD.begin(11)) {
    SerialUSB.println("SD >> Card failed, or not present");
    while(1) {}
  }

  file = SD.open("test.txt", FILE_WRITE);

  radio.begin();
}

void loop() {
  // send by radio if empty
  if (radio.tx_fifo_empty()) {
    SerialUSB.println("Sending frame");
    radio.transmit("Hello CanSat!");
  }

  // save on SD
  SerialUSB.println("Saving on SD");
  file.println("Hello CanSat!");

  // blink!
  if (millis() - last_blink >= 250)
  {
    digitalWrite(Pins::LED, led_state);
    led_state = !led_state;
    last_blink = millis();
  }
}
