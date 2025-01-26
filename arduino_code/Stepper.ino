/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <Test for Accessories library, with one stepper motor controled by Dcc.>
*************************************************************/

#include "Commanders.h"
#include "Accessories.h"

// DCC codes
#define POS1	1
#define POS2	2
#define POS3	3
#define POS4	4
#define POS5	5

// Commanders

SERIAL_COMMANDER(Serial)

// Accessories

uint8_t steps28BYJ48[4] =
{
    B1100,
    B0110,
    B0011,
    B1001,
};

AccessoryStepper stepper;

// Drivers

PortStepper portStepper;

void ReceiveEvent(unsigned long inId, COMMANDERS_EVENT_TYPE inEventType, int inEventData)
{
	Accessories::ReceiveEvent(inId, (ACCESSORIES_EVENT_TYPE)inEventType, inEventData);
}

//////////////////////////////////
//
// Setup
//
void setup()
{
	Serial.begin(115200);
	//while(!Serial);		// For Leonardo only. No effect on other Arduino.

	Commanders::begin(ReceiveEvent, LED_BUILTIN);
	Accessories::begin();

	// Commanders setup

	SerialCommander.begin();
	
	// Drivers setups

    // one light is connected to the arduino.
	portStepper.begin(1, 2, 3, 4, DIGITAL, (uint8_t *) steps28BYJ48);
	
	// Accessories setups

    // Assign Dcc code for each accessory.
	stepper.begin(&portStepper, 64, 32, 200, 5);

	stepper.AddMovingPosition(1, -1024);
	stepper.AddMovingPosition(2, -512);
	stepper.AddMovingPosition(3, 0);
	stepper.AddMovingPosition(4, 512);
	stepper.AddMovingPosition(5, 1024);
}

void loop()
{
	Accessories::loop();

	Commanders::loop();
}
