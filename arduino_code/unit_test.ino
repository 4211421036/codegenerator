// Arduino RBD Threshold Library v1.0.3 - Unit test coverage.
// https://github.com/alextaujenis/RBD_Threshold
// Copyright (c) 2015 Alex Taujenis - MIT License

#include <ArduinoUnit.h>   // https://github.com/mmurdoch/arduinounit
#include <RBD_Threshold.h> // https://github.com/alextaujenis/RBD_Threshold

RBD::Threshold threshold(3);

void testCleanup() {
  // set at values far above the tested range
  threshold.setLevel(1,10001);
  threshold.setLevel(2,10002);
  threshold.setLevel(3,10003);
  threshold.setMaxLevel(10004);
}

// setLevel
test(setLevel_should_set_the_level) {
  threshold.setLevel(1, 100);

  assertEqual(threshold.getLevel(1), 100);
  testCleanup();
}

test(setLevel_should_set_the_second_level) {
  threshold.setLevel(2, 200);

  assertEqual(threshold.getLevel(2), 200);
  testCleanup();
}

// getLevel
test(getLevel_should_return_the_level) {
  threshold.setLevel(3,300);

  assertEqual(threshold.getLevel(3), 300);
  testCleanup();
}

// setMaxLevel
test(getLevel_should_set_the_max_level) {
  threshold.setMaxLevel(500);

  assertEqual(threshold.getMaxLevel(), 500);
  testCleanup();
}

// getMaxLevel
test(getMaxLevel_should_return_the_max_level) {
  threshold.setMaxLevel(501);

  assertEqual(threshold.getMaxLevel(), 501);
  testCleanup();
}

// computeLevel
test(computeLevel_should_return_zero_if_value_is_below_the_first_level) {
  threshold.setLevel(1,100);

  assertEqual(threshold.computeLevel(99), 0);
  testCleanup();
}

test(computeLevel_should_return_one_if_value_is_equal_to_the_first_level) {
  threshold.setLevel(1,100);

  assertEqual(threshold.computeLevel(100), 1);
  testCleanup();
}

test(computeLevel_should_return_one_if_value_is_between_the_first_and_second_level) {
  threshold.setLevel(1,100);
  threshold.setLevel(2,200);

  assertEqual(threshold.computeLevel(101), 1);
  testCleanup();
}

test(computeLevel_should_return_one_if_value_is_just_below_the_second_level) {
  threshold.setLevel(1,100);
  threshold.setLevel(2,200);

  assertEqual(threshold.computeLevel(199.99), 1);
  testCleanup();
}

test(computeLevel_should_return_two_if_value_is_equal_to_the_second_level) {
  threshold.setLevel(1,100);
  threshold.setLevel(2,200);

  assertEqual(threshold.computeLevel(200), 2);
  testCleanup();
}

test(computeLevel_should_return_two_if_value_is_just_above_the_second_level) {
  threshold.setLevel(1,100);
  threshold.setLevel(2,200);

  assertEqual(threshold.computeLevel(200.01), 2);
  testCleanup();
}

test(computeLevel_should_return_three_if_value_is_at_the_third_level) {
  threshold.setLevel(1,100);
  threshold.setLevel(2,200);
  threshold.setLevel(3,300);

  assertEqual(threshold.computeLevel(300), 3);
  testCleanup();
}

test(computeLevel_should_return_the_last_level_if_the_value_is_equal_to_max_level) {
  threshold.setLevel(1,100);
  threshold.setLevel(2,200);
  threshold.setLevel(3,300);
  threshold.setMaxLevel(350);

  assertEqual(threshold.computeLevel(350), 3);
  testCleanup();
}

test(computeLevel_should_return_four_if_value_is_greater_than_the_thrid_level) {
  threshold.setLevel(1,100);
  threshold.setLevel(2,200);
  threshold.setLevel(3,300);
  threshold.setMaxLevel(350);

  assertEqual(threshold.computeLevel(351), 4);
  testCleanup();
}

test(computeLevel_should_return_four_if_value_is_much_greater_than_the_thrid_level) {
  threshold.setLevel(1,100);
  threshold.setLevel(2,200);
  threshold.setLevel(3,300);
  threshold.setMaxLevel(350);

  assertEqual(threshold.computeLevel(1000), 4);
  testCleanup();
}


// setup and run tests
void setup() {
  Serial.begin(115200);
  while(!Serial);
}

void loop() {
  Test::run();
}