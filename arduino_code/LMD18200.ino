/*************************************************************
project: <Accessoriesr>
author: <Thierry PARIS>
description: <One DC motor by a LMD18200>
*************************************************************/

#include "Commanders.h"
#include "Accessories.h"

// DCC codes
#define MOTOR_LEFT		DCCINT(20, 0)
#define MOTOR_RIGHT		DCCINT(21, 0)
#define MOTOR_STOP		DCCINT(21, 1)

// Commanders

SERIAL_COMMANDER(Serial);

#ifdef VISUALSTUDIO
ButtonsCommanderKeyboard	stop;
ButtonsCommanderKeyboard	left;
ButtonsCommanderKeyboard	right;
#else
ButtonsCommanderPush stop;
ButtonsCommanderPush left;
ButtonsCommanderPush right;

#endif

// Accessories

AccessoryMotorTwoWays motor;

// Ports

PortSpeedDirBrake port;

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
	//while (!Serial);		// For Leonardo only. No effect on other Arduino.

	Commanders::begin(ReceiveEvent, LED_BUILTIN);
	Accessories::begin();

	// Commanders setup

	SerialCommander.begin();
	DccCommander.begin(0x00, 0x00, digitalPinToInterrupt(3));

#ifdef VISUALSTUDIO
	stop.begin(MOTOR_STOP, '5');
	left.begin(MOTOR_LEFT, '4');
	right.begin(MOTOR_RIGHT, '6');
#else
	stop.begin(MOTOR_STOP, 24);
	left.begin(MOTOR_LEFT, 25);
	right.begin(MOTOR_RIGHT, 26);
#endif

	// Drivers setups

	port.begin(10, 8, 9, DIGITAL, DIGITAL);
	
	// Accessories setups

    // Declare accessories.
	motor.beginTwoWays(&port, MOTOR_RIGHT, MOTOR_LEFT, 5, 0);
	motor.AddMovingPosition(MOTOR_STOP, STATE_NONE);
}

void loop()
{
	Accessories::loop();

	Commanders::loop();
}
