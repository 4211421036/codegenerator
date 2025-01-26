#line 2 "first.ino"
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

  SerialUSB.println("Starting first! Now run second.ino");

  char tmp[255];
  radio.receive(tmp);
  assertEqual(strlen(tmp), 2);
  assertEqual(tmp[0], 'G');
  assertEqual(tmp[1], 'O');
  delay(100);
}

void check_empty() {
  delay(3000);
  assertFalse(radio.available());
  while(radio.available()) {
    uint8_t data[256];
    uint8_t length;
    radio.receive(data, length);
    SerialUSB.print("GOT ");
    SerialUSB.print(length);
    SerialUSB.print(": ");
    for(int i = 0; i < length; ++i) {
      uint8_t value = data[i];
      SerialUSB.print(value);
      SerialUSB.print(", ");
    }
    SerialUSB.println();
  }
}

void test_setup() {
  radio.transmit("RUN");
}

void test_cleanup() {
  check_empty();
  radio.transmit("OK");
}


#define CANSAT_TEST(name) void CANSAT_TEST_ ## name(); test(name) {test_setup(); CANSAT_TEST_ ## name(); test_cleanup();} void CANSAT_TEST_ ## name()

// tests

test(0_no_rssi_measured) {
  auto last = radio.get_rssi_last();
  
  // already frame received in setup
  assertLess(last, -40);
  assertMore(last, -140);
}

CANSAT_TEST(1_receive_string) {
  const char * strings_to_receive[] = {
    "a",
    "ABC",
    "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`ab",
    "test transmission 157.65",
    "187",
    "187 tester",
    "187 testerfoo 2.56",
    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
    "abc 12 123.98\n"
  };

  for (int test = 0; test < sizeof(strings_to_receive)/sizeof(strings_to_receive[0]); ++test) {
    SerialUSB.print("Case ");
    SerialUSB.println(test);

    auto str = strings_to_receive[test];
    auto len = strlen(str);

    char data[256];
    for (int i = 0; i < 256; ++i) {
      data[i] = 'q';
    }
    
    radio.receive(data);
    SerialUSB.println(data);

    for (int i = 0; i < len; ++i) {
      SerialUSB.print(i);
      SerialUSB.print(' ');
      SerialUSB.print(data[i]);
      SerialUSB.print(' ');
      SerialUSB.println(str[i]);
      assertEqual(data[i], (char)str[i]);
    }
    assertEqual(data[len], '\0');
    for (int i = len+1; i < 256; ++i) {
      assertEqual(data[i], 'q');
    }
  }
}

CANSAT_TEST(2_receive_bytes) {
  constexpr static auto nr_of_tests = 3;
  const uint8_t bytes_to_receive[nr_of_tests][256] = {
    {3, 0, 1, 2},
    {1, 3},
    {255, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
  };

  for (int test = 0; test < nr_of_tests; ++test) {
    SerialUSB.print("Case ");
    SerialUSB.println(test);

    uint8_t data[256];
    uint8_t length = 178;
    for (int j = 0; j < 256; ++j) {
      data[j] = 'q';
    }
    
    radio.receive(data, length);

    assertEqual(length, bytes_to_receive[test][0]);

    for (int i = 0; i < length; ++i) {
      assertEqual(data[i], bytes_to_receive[test][i+1]);
    }

    for (int i = length; i < 256; ++i) {
      assertEqual(data[i], 'q');
    }
  }
}

CANSAT_TEST(3_fill_buffer) {
  while (radio.available() != 10);

  for (int test = 0; test < 11; ++test) {
    assertEqual(radio.available(), 10-test);

    SerialUSB.print("Case ");
    SerialUSB.println(test);

    uint8_t data[255];
    uint8_t length = 47;
    radio.receive(data, length);

    assertEqual(length, 255);

    for (int i = 0; i < length; ++i) {
      assertEqual(data[i], i % 0xFF);
    }
  }
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
  while (radio.available() != 1);
  assertEqual(radio.available(), 1);

  char data[256];
  radio.receive(data);

  check_empty();

  radio.transmit(data);
  
  radio.flush();
}

CANSAT_TEST(6_rx_buffer_overflow) {
  while (radio.available() != 10);
  delay(2000);

  for (int test = 0; test < 5; ++test) {
    assertEqual(radio.available(), 10-test);

    SerialUSB.print("Case ");
    SerialUSB.println(test);

    uint8_t data[255];
    uint8_t length = 47;
    radio.receive(data, length);

    assertEqual(length, 255);

    for (int i = 0; i < length; ++i) {
      assertEqual(data[i], i % 0xFF);
    }
  }

  while (radio.available() != 6);
  assertEqual(radio.available(), 6);

  for (int test = 0; test < 5; ++test) {
    assertEqual(radio.available(), 6-test);

    SerialUSB.print("Case ");
    SerialUSB.println(test);

    uint8_t data[255];
    uint8_t length = 47;
    radio.receive(data, length);

    assertEqual(length, 255);

    for (int i = 0; i < length; ++i) {
      assertEqual(data[i], i % 0xFF);
    }
  }

  assertEqual(radio.available(), 1);
  char data[256];
  radio.receive(data);
  assertEqual(0, strcmp(data, "pass"));
}

void loop() {
  Test::run();
}
