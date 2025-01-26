#include "Arduino_EMBRYO_2.h"

/******* I N I T I A L   D E F I N I T I O N S ********/
#define motor01 1
#define MIN_SPEED 25000
#define MAX_SPEED 200

/* C O N S T A N T S   A N D   V A R I A B L E S */
const int enablePin = A5;     // Enable Pin
const int DirPin = 12;        // Direction Pin for motor01
const int PulPin = A4;        // Step Pin for motor01
const int ForwardPin = A2;    // Forward Button for motor01
const int BackwardPin = A1;   // Backward Button for motor01

const int startPin = 2;       // Start Button
const int emergencyPin = 5;   // Emergency Button
const int HomeEndstop = 3;    // Home endstop
const int FarEndstop = 4;     // Far From Home endstop

int potentiometer = A0;       // Potentiometer pin
int speedMotor = 200;         // Initial speed (Max speed = 200 and Min speed = 25000)

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

  motor.startWithoutHoming(); // Initialize the motor without homing procedure, detached
                              // interruptions pins and configures endstops as OUTPUT
                              // Use this function only with the motor outside the axis
  
  Serial.println("To start, send any key to serial port ...");
  // Without this instruction the motor will move after the upload (It is dangerous)
  // Wait for data in the serial port
  while((Serial.available() <= 0)){};
  Serial.read();              // Clean serial buffer

  Serial.println("Use the potentiometer to control the speed.");
}
void loop() {
  // read the input pin
  speedMotor = analogRead(potentiometer);

  // Re-maps an analog value to speed value
  speedMotor = map(speedMotor, 0, 1023, MIN_SPEED, MAX_SPEED);

  Serial.println(speedMotor); // Send the new value to serial monitor

  motor.setSpeed(speedMotor); // Sets the motor speed
  
  // Check the forward button signal
  if(motor.readBtnForward() == HIGH)
    motor.moveForward();      // Motor rotates clockwise
}
