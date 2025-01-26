// declare global variables and constants that will be used later in the code
int counter = 1;
bool led_state = false;
const int led_pin = 13;


// body of setup() function executes only once at the beginning of the program
void setup() {
  // setup communication via USB Serial port
  SerialUSB.begin(115200);

  // setup pin with attached LED as OUTPUT
  pinMode(led_pin, OUTPUT);
}

// body of loop() function executes infinitely in the loop
void loop() {
  // set state of pin with attached diode
  // when led_state is true LED is on, when false - LED is switched off
  digitalWrite(led_pin, led_state);

  // change led_state to opposite value (false => true; true => false)
  led_state = !led_state;

  // print to SerialUSB (Serial Monitor in Arduino IDE) value of counter variable
  SerialUSB.print(counter);

  // increment (i.e. add one) current value of counter variable
  counter++;

  // print to SerialUSB ". Hello CanSat!" message and add new line (println)
  SerialUSB.println(". Hello CanSat!");

  // wait for 1 second (= 1000 ms)
  delay(1000);
}
