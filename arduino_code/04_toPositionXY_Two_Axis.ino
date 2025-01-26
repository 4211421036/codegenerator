#include "Arduino_EMBRYO_2.h"
/* I N I T I A L    D E F I N E S */
#define X_AXIS 1
#define Y_AXIS 2

/*C O N S T A N T S   A N D   V A R I A B L E S*/
const int enablePin = A5;     // Enable Pin
const int DirPinX = 12;       // Direction Pin X-axis
const int PulPinX = A4;       // Step Pin X-axis
const int HomeEndstopX = 3;   // Home endstop X-axis
const int FarEndstopX = 4;    // Far From Home endstop X-axis
const int BackwardPinX = A1;  // Backward Button X-axis
const int ForwardPinX = A2;   // Forward Button X-axis

const int DirPinY = 10;       // Direction Pin Y-axis
const int PulPinY = 11;       // Step Pin Y-axis
const int HomeEndstopY = 8;   // Home endstop Y-axis
const int FarEndstopY = 7;    // Far From Home endstop Y-axis
const int BackwardPinY = A3;  // Backward Button Y-axis
const int ForwardPinY = 6;    // Forward Button Y-axis

const int startPin = 2;       // Start Button
const int emergencyPin = 5;   // Emergency Button

unsigned int positionX = 0, positionY = 0;

// Construct object, StepMotor(Axis, Enable Pin, Direction Pin, Pulse Pin, Endstop Home, Endstop Far, Forward Button, Backward Button, Start Button, Emergency Stop Button)
StepMotor axisX(X_AXIS,
                enablePin,
                DirPinX,
                PulPinX,
                HomeEndstopX,
                FarEndstopX,
                ForwardPinX,
                BackwardPinX,
                startPin,
                emergencyPin);

StepMotor axisY(Y_AXIS,
                enablePin,
                DirPinY,
                PulPinY,
                HomeEndstopY,
                FarEndstopY,
                ForwardPinY,
                BackwardPinY,
                startPin,
                emergencyPin);

// Construct object, Embryo(axisX, axisY, Start Button, Emergency Stop Button)
Embryo robot(axisX, axisY, startPin, emergencyPin);

void setup()
{
  Serial.begin(9600);         // Configure and start Serial Communication
  while (!Serial)
  {
  };                          // Wait to open the serial monitor

  robot.begin();              // Configure the two-axis robot inputs pins, outputs pins, and interruptions pins

  Serial.println("Press the Start Button to start the machine");
  while (!robot.ready())
    ;                         // Wait for Start button to be pressed
                              // The start button is attached to the interrupt
                              // service routine that enables the motor and runs
                              // the homing procedure

  Serial.println("Move the axis between to minimum position 0cm and the maximum position 45cm");
}
void loop()
{
  // put your main code here, to run repeatedly:
  Serial.println("Enter the number of the position for X-axis: ");
  // Waits for the user to send a value via serial port
  while ((Serial.available() <= 0))
  {
  };
  // Reads the value in the serial port
  positionX = Serial.parseInt();

  Serial.println("Enter the number of the position for Y-axis: ");
  // Waits for the user to send a value via serial port
  while ((Serial.available() <= 0))
  {
  };
  // Reads the value in the serial port
  positionY = Serial.parseInt();

  // Print the axis position before running the command
  Serial.println("Position before - X-axis: " + String(axisX.getPosition()));
  Serial.println("Position before - Y-axis: " + String(axisY.getPosition()));

  // Runs the command
  robot.toPositionXY(positionX, positionY);

  // Print the axis position after running the command
  Serial.println("Position after - X-axis: " + String(axisX.getPosition()));
  Serial.println("Position after - Y-axis: " + String(axisY.getPosition()));
}
