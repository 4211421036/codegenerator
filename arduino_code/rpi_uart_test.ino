// This program sets up serial communication on the Teensy and repeatedly sendsa message over the UART connection. Use minicom on the RPi to see the message.

void setup()
{
    Serial2.begin(115200); // Begin serial communication on Teensy's UART2 port
}

void loop()
{
    Serial2.println("Hello, Raspberry Pi!"); // Send the message over UART
    delay(1000);                             // Wait for 1 second before sending the next message
}
