// Global Variables used in the setup() and loop()
int delay_duration = 30;

// Initial setup to prepare loop()
void setup() {
    //initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    // Increasing brightness
    for (int brightness = 0; brightness <= 255; brightness += 5) {
        analogWrite(LED_BUILTIN, brightness); //Control brightness using Arduino-supported PWM
        delay(delay_duration);
    }

    // Decreasing brightness
    for (int brightness = 255; brightness >= 0; brightness -= 5) {
        analogWrite(LED_BUILTIN, brightness); 
        delay(delay_duration);
    }
}