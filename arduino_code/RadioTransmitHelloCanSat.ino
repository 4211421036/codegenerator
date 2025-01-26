#include <CanSatKit.h>

using namespace CanSatKit;

int counter = 1;
bool led_state = false;
const int led_pin = 13;

Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,
            Bandwidth_125000_Hz,
            SpreadingFactor_9,
            CodingRate_4_8);

// create (empty) radio frame object that can store data
// to be sent via radio
Frame frame;

void setup() {
  SerialUSB.begin(115200);
  pinMode(led_pin, OUTPUT);

  radio.begin();
}

void loop() {
  digitalWrite(led_pin, led_state);
  led_state = !led_state;

  // store ("print") value of counter variable into radio frame
  frame.print(counter);
  // increment counter variable
  counter++;

  // store ". Hello CanSat!" message in radio frame
  // (append to previous content)
  frame.print(". Hello CanSat!");

  // send frame via radio
  radio.transmit(frame);

  // print frame also on SerialUSB
  SerialUSB.println(frame);

  // clear frame to make it ready for new data
  frame.clear();

  // wait for 1 s
  delay(1000);
}