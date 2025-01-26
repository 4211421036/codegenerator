// Global Variables used in the setup() and loop()
int delay_duration = 1000;

// Initial setup to prepare loop()
void setup() {
    //initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(delay_duration);  // wait for a second
    digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
    delay(delay_duration);  // wait for a second
}