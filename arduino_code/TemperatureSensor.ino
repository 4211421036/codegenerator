#include <cmath>

// port where LM35 temperature sensor is connected
const int lm35_pin = A0;

// a function that calculates temperature (in *C) from analogRead raw reading
// see datasheet of LM35 and analogRead Arduino function documentation
float lm35_raw_to_temperature(int raw) {
  float voltage = raw * 3.3 / (std::pow(2, 12));
  float temperature = 100.0 * voltage;
  return temperature;
}

void setup() {
  SerialUSB.begin(115200);

  // set the best resolution of measurement available for analogRead
  // it's 12 bits for SAM21D-based Arduino boards
  analogReadResolution(12);
}

void loop() {
  // read raw analog value from lm35_pin analog input
  int raw = analogRead(lm35_pin);

  // use lm35_raw_to_temperature function to calculate temperature
  float temperature = lm35_raw_to_temperature(raw);

  // print temperature on SerialUSB
  SerialUSB.print("Temperature = ");
  SerialUSB.print(temperature);
  SerialUSB.println(" deg C");

  delay(50);
}
