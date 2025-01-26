#line 2 "second.ino"
#include <CanSatKit.h>
#include <ArduinoUnit.h>

using namespace CanSatKit;

// set radio receiver parameters - see comments below
// remember to set the same radio parameters in
// transmitter and receiver boards!
Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,                  // frequency in MHz
            Bandwidth_125000_Hz,    // bandwidth - check with CanSat regulations to set allowed value
            SpreadingFactor_7,      // see provided presentations to determine which setting is the best
            CodingRate_4_8);        // see provided presentations to determine which setting is the best

void setup() {
  SerialUSB.begin(115200);
  Test::out = &SerialUSB;

  while(!SerialUSB);

  // start radio module
  if (!radio.begin()) {
    Test::abort();
  }

  SerialUSB.println("Starting second!");

  radio.transmit("GO");
}


void wait_for_answer() {
  char tmp[255];
  radio.receive(tmp);
  assertEqual(strlen(tmp), 2);
  assertEqual(tmp[0], 'O');
  assertEqual(tmp[1], 'K');
  delay(100);
}

void wait_for_start() {
  char tmp[255];
  radio.receive(tmp);
  assertEqual(strlen(tmp), 3);
  assertEqual(tmp[0], 'R');
  assertEqual(tmp[1], 'U');
  assertEqual(tmp[2], 'N');
  delay(100);
}

void test_setup() {
  wait_for_start();
}

void test_cleanup() {
  radio.flush();
  wait_for_answer();
}


#define CANSAT_TEST(name) void CANSAT_TEST_ ## name(); test(name) {test_setup(); CANSAT_TEST_ ## name(); test_cleanup();} void CANSAT_TEST_ ## name()

// tests
test(0_no_rssi_measured) {
  auto last = radio.get_rssi_last();
  
  assertEqual(last, -164);
}

CANSAT_TEST(1_receive_string) {
  assertTrue(radio.transmit("a"));

  const char* tmp = "ABC";
  assertTrue(radio.transmit(tmp));

  const char* tmp2 = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abc";
  assertEqual(strlen(tmp2), 255);
  assertFalse(radio.transmit(tmp2));
  char tmp3[256];
  strcpy(tmp3, tmp2);
  tmp3[254] = '\0';
  assertEqual(strlen(tmp3), 254);
  assertTrue(radio.transmit(tmp3));

  Frame frame;
  frame.print("test");
  frame.print(' ');
  frame.print("transmis");
  frame.print("sion");
  frame.print(' ');
  volatile float x = 157.65;
  frame.print(x);
  assertTrue(radio.transmit(frame));

  frame.clear();
  frame.print(187);
  assertTrue(radio.transmit(frame));

  frame.print(" tester");
  assertTrue(radio.transmit(frame));

  const char* tmp4 = "foo";
  float tmp5 = 2.56;
  frame.print(tmp4);
  frame.print(' ');
  frame.print(tmp5);
  assertTrue(radio.transmit(frame));

  frame.clear();
  assertEqual(254, frame.print("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));
  assertEqual(frame.size, 254);
  assertTrue(radio.transmit(frame));
  assertEqual(0, frame.println());
  assertEqual(frame.size, 254);

  String string;
  string += "abc";
  string += ' ';
  int tmp6 = 18;
  string += String(tmp6, HEX);
  string += String(' ');
  string += '1';
  float f = 23.98;
  string += String(f);
  string += '\n';
  assertTrue(radio.transmit(string));
}

CANSAT_TEST(2_receive_bytes) {
  uint8_t buffer[300];
  
  buffer[0] = 0;
  buffer[1] = 1;
  buffer[2] = 2;
  assertTrue(radio.transmit(buffer, 3));

  uint8_t tmp = 3;
  assertTrue(radio.transmit(&tmp, 1));

  assertFalse(radio.transmit(nullptr, 0));

  for (int i = 0; i < sizeof(buffer); ++i) {
    buffer[i] = 1;
  }

  radio.transmit(buffer, 255);
}

CANSAT_TEST(3_fill_buffer) {
  uint8_t buffer[255];
  for (int i = 0; i < sizeof(buffer); ++i) {
    buffer[i] = i % 0xFF;
  }

  // one frame goes directly to the radio module
  // 10 frames to the buffer
  for (int i = 0; i < 11; ++i) {
    assertTrue(radio.transmit(buffer, 255));
  }
  // not enough space for last one
  assertFalse(radio.transmit(buffer, 255));
}

CANSAT_TEST(4_rssi) {
  auto last = radio.get_rssi_last();
  auto now = radio.get_rssi_now();
  
  assertLess(now, -40);
  assertLess(last, -40);

  assertMore(now, -140);
  assertMore(last, -140);
}

CANSAT_TEST(5_bidirectional) {
  const char* str = "test 123";

  radio.transmit(str);
  assertEqual(radio.available(), 0);

  char data[256];
  radio.receive(data);
  assertEqual(radio.available(), 0);

  assertEqual(0, strcmp(str, data));
}

CANSAT_TEST(6_rx_buffer_overflow) {
  uint8_t buffer[255];
  for (int i = 0; i < sizeof(buffer); ++i) {
    buffer[i] = i % 0xFF;
  }

  for (int i = 0; i < 10; ++i) {
    assertTrue(radio.transmit(buffer, 255));
  }
  radio.flush();
  radio.transmit("TEST_SHOULD_BE_DROPPED");
  radio.flush();

  delay(4000);
  radio.transmit("pass");
}

void loop() {
  Test::run();
}
