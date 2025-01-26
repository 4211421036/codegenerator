// Arduino RBD Threshold Library v1.0.3 Example - Convert an input domain to a numeric quantile output range.
// https://github.com/alextaujenis/RBD_Threshold
// Copyright (c) 2015 Alex Taujenis - MIT License

#include <RBD_Threshold.h> // https://github.com/alextaujenis/RBD_Threshold

RBD::Threshold threshold(3); // 3 levels

void setup() {
  Serial.begin(115200);
  threshold.setLevel(1,0);    // low
  threshold.setLevel(2,10.5); // medium
  threshold.setLevel(3,20);   // high
  threshold.setMaxLevel(30);
}

void loop() {
  Serial.println(threshold.computeLevel(-1));
  Serial.println(threshold.computeLevel(8));
  Serial.println(threshold.computeLevel(10.6));
  Serial.println(threshold.computeLevel(30));
  Serial.println(threshold.computeLevel(31));
}