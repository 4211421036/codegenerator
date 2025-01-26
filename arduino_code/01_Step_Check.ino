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

// Construct object, Embryo(Axis, Enable Pin, Direction Pin, Pulse Pin, Endstop Home, Endstop Far, Forward Button, Backward Button, Start Button, Emergency)
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
  motor.startWithoutHoming(); // Initialize the motor separately without being part of an axis

  Serial.println("To start, send any key to serial ...");

  // Without this instruction the motor will move after the upload (It is dangerous)
  while((Serial.available() <= 0)){};

  motor.setTotalSteps(50000); // Set the max step of the motor
  Serial.println("Motor moves.");
  motor.moveSteps(20000);     // Motor moves

  motor.pause();              // Pause the motor
  Serial.println("Motor does not move.");
  motor.moveSteps(50);        // Motor does not move
  delay(2000);
  
  motor.play();               // Enable motor
  Serial.println("Motor moves.");
  motor.moveSteps(15000);     // Motor moves again
}
void loop() {}
