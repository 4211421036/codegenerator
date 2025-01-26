#include "Arduino_EMBRYO_2.h"

/* I N I T I A L   D E F I N I T I O N S */
#define motor01      1

/* C O N S T A N T S   A N D   V A R I A B L E S */
const int enablePin = A5;      // Enable Pin
const int DirPin = 12;         // Direction Pin for motor01
const int PulPin = A4;         // Step Pin for motor01
const int ForwardPin  = A2;    // Forward Button for motor01
const int BackwardPin = A1;    // Backward Button for motor01

const int startPin = 2;        // Start Button
const int emergencyPin  = 5;   // Emergency Button
const int HomeEndstop = 3;     // Home endstop
const int FarEndstop = 4;      // Far From Home endstop

long steps = 0;

// Construct object, Embryo(id, Enable Pin, Direction Pin, Pulse Pin, Endstop Home, Endstop Far, Forward Button, Backward Button, Start Button, Emergency)
StepMotor motor(motor01,
                enablePin,
                DirPin,
                PulPin,
                HomeEndstop,
                FarEndstop,
                ForwardPin,
                BackwardPin,
                startPin,
                emergencyPin);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);         // Configure and start Serial Communication
  while (!Serial) {};         // Wait to open the serial monitor
  motor.begin();              // Configure inputs pins, outputs pins and interruptions pins
  motor.startWithoutHoming(); // Initialize the motor without homing procedure, detached
                              // interruptions pins and configures endstops as OUTPUT
                              // Use this function only with the motor outside the axis
  motor.setTotalSteps(50000); // Set the max step of the motor (necessary, because the total setp count is initialized with zero)
} 

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Enter the number of steps: ");

  // Waits for the user to send a value via serial port
  while((Serial.available() <= 0)){};
  steps = Serial.parseInt();  // Reads the value in the serial port

  // Print the current step value before running the command
  Serial.println("Currentstep before running the command: " + String(motor.getStep()));
  
  motor.moveSteps(steps);     // Runs the command

  // Print the current step value after running the command
  Serial.println("Curentstep after running the command: " + String(motor.getStep()));
  
  Serial.read();              // Clean serial buffer
}
