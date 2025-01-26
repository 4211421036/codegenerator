/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <Test for Accessories library, with one light, controled by Dcc.>
*************************************************************/

#include "Commanders.h"
#include "Accessories.h"

// DCC codes
#define ACTION		DCCINT(20,0)

// Commanders

SERIAL_COMMANDER(Serial)

// Accessories

AccessoryLight  light;

// Ports
	
PortOnePin port;

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

	// Drivers setups

    // one light is connected to the arduino.
	port.begin(12, ANALOG);
	
	// Accessories setups

    // Assign Dcc code for each accessory.
	light.begin(&port, ACTION);
	light.SetBlinking(3000);
	//light.SetFading(20, 20);
}

void loop()
{
	Accessories::loop();

	Commanders::loop();
}