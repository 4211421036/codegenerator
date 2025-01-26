/* With this code, you can regulate the power state of your Raspberry Pi Zero using the serial monitor.
By sending the commands "ON" or "OFF" via the serial monitor, you can turn the Raspberry Pi on or off.
The program reads commands from the serial monitor, processes them, and sets the power state accordingly by controlling a GPIO pin connected to the Raspberry Pi.*/

#include <Arduino.h>

// Pin connected to the Raspberry Pi's GPIO
const int RASPBERRY_PI_GPIO_PIN = 36;

// Command buffer
String command = "";

void setup()
{
    // Initialize serial communication
    Serial.begin(115200);

    // Configure the Raspberry Pi control pin
    pinMode(RASPBERRY_PI_GPIO_PIN, OUTPUT);

    // Set Raspberry Pi to off state initially
    digitalWrite(RASPBERRY_PI_GPIO_PIN, LOW);

    Serial.println("Teensy 4.1 Raspberry Pi Control");
    Serial.println("Commands: ON, OFF");
}

void loop()
{
    // Read commands from the serial monitor
    if (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n')
        {
            processCommand(command);
            command = "";
        }
        else
        {
            command += c;
        }
    }
}

void processCommand(String cmd)
{
    cmd.trim(); // Remove any leading or trailing whitespace

    // Turn the Raspberry Pi on or off based on the command
    if (cmd.equalsIgnoreCase("ON"))
    {
        digitalWrite(RASPBERRY_PI_GPIO_PIN, HIGH);
        Serial.println("Raspberry Pi turned ON");
    }
    else if (cmd.equalsIgnoreCase("OFF"))
    {
        digitalWrite(RASPBERRY_PI_GPIO_PIN, LOW);
        Serial.println("Raspberry Pi turned OFF");
    }
    else
    {
        Serial.println("Invalid command. Please enter ON or OFF.");
    }
}
