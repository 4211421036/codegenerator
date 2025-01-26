/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <Test for fading light with just one button>
*************************************************************/

#include "Commanders.h"
#include "Accessories.h"

// DCC codes
#define RED_TOGGLE		DCCINT(20,0)
#define GREEN_TOGGLE	DCCINT(20,1)

// Commanders

#ifdef VISUALSTUDIO
ButtonsCommanderKeyboard	push;
#else
ButtonsCommanderPush push;
#endif

SERIAL_COMMANDER(Serial);

// Accessories

AccessoryLight red, green;

// Drivers
	
PortOnePin Port0;
PortOnePin Port1;

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
	while (!Serial);		// For Leonardo only. No effect on other Arduino.

	Commanders::begin(ReceiveEvent, LED_BUILTIN);
	Accessories::begin();

	// Commanders setup

	SerialCommander.begin();

	// One button
    // This button will send commands to Dcc code 20/0 and 20/1, on pin 26
#ifdef VISUALSTUDIO
	push.begin(RED_TOGGLE, '0');
#else
	push.begin(RED_TOGGLE, 26);
#endif
	push.AddEvent(GREEN_TOGGLE);

	// Drivers setups

    // Two ports of the Arduino used by the leds, 10 and 11
	Port0.begin(10, ANALOG_INVERTED);
	Port1.begin(11, ANALOG_INVERTED);

	// Accessories setups

    // Two lights in the accessory list.

	red.begin(&Port0, RED_TOGGLE, 0, 120);
	green.begin(&Port1, GREEN_TOGGLE, 0, 200);

    // Define fading/dimming effect
	red.SetFading(20, 20);
	green.SetFading(20, 20);

    // Start with light0 on !
	red.LightOn();
}

void loop()
{
	Accessories::loop();

	Commanders::loop();
}