/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <Demo for groups>
*************************************************************/

#include "Commanders.h"
#include "Accessories.h"

///////////////////////////////////////////////////////////////
// The target is to simulate a traffic light with four leds.
// A group defines the traffic light, which link four light accessories.
// Four different states defines which light must be on, which must be off.
// Only one button is used to move from one state to the following,
// returning to the first one when finished.

// DCC codes
#define ALLON			DCCINT(20,0)
#define ALLOFF			DCCINT(20,1)
#define ALLONFIRSTBLINK	DCCINT(21,0)
#define ALLOFFLASTBLINK	DCCINT(21,1)

#define RED_TOGGLE		DCCINT(22,0)
#define ORANGE_TOGGLE	DCCINT(22,1)
#define GREEN_TOGGLE	DCCINT(23,0)
#define WHITE_TOGGLE	DCCINT(23,1)

// Commanders

#ifdef VISUALSTUDIO
ButtonsCommanderKeyboard	push;
#else
ButtonsCommanderPush push;
#endif

SERIAL_COMMANDER(Serial);

// Accessories

AccessoryGroup group;

AccessoryLight red, green, orange, white;

// Ports
	
PortOnePin PortRed;
PortOnePin PortOrange;
PortOnePin PortGreen;
PortOnePin PortWhite;

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

#ifdef VISUALSTUDIO
	push.begin(ALLON, '0');
#else
	push.begin(ALLON, 17);
#endif
	push.AddEvent(ALLOFF);
	push.AddEvent(ALLONFIRSTBLINK);
	push.AddEvent(ALLOFFLASTBLINK);

	// Ports setups

	// Wire leds to pins 8, 9, 10, 11
	 
	PortRed.begin(8, DIGITAL);
	PortOrange.begin(9, DIGITAL);
	PortGreen.begin(10, DIGITAL);
	PortWhite.begin(11, DIGITAL);
	
	// Buttons setups

	// Accessories setups

	red.begin(&PortRed, RED_TOGGLE, 500);
	orange.begin(&PortOrange, ORANGE_TOGGLE);
	green.begin(&PortGreen, GREEN_TOGGLE);
	white.begin(&PortWhite, WHITE_TOGGLE, 500);

	// Describe the four differents states
	group.begin();

	group.AddState(ALLON, false);
	group.AddStateItem(ALLON, red, LIGHTON);
	group.AddStateItem(ALLON, orange, LIGHTON);
	group.AddStateItem(ALLON, green, LIGHTON);
	group.AddStateItem(ALLON, white, LIGHTON);

	group.AddState(ALLOFF, false);
	group.AddStateItem(ALLOFF, red, LIGHTOFF);
	group.AddStateItem(ALLOFF, orange, LIGHTOFF);
	group.AddStateItem(ALLOFF, green, LIGHTOFF);
	group.AddStateItem(ALLOFF, white, LIGHTOFF);

	group.AddState(ALLONFIRSTBLINK, false);
	group.AddStateItem(ALLONFIRSTBLINK, red, LIGHTBLINK);
	group.AddStateItem(ALLONFIRSTBLINK, orange, LIGHTON);
	group.AddStateItem(ALLONFIRSTBLINK, green, LIGHTON);
	group.AddStateItem(ALLONFIRSTBLINK, white, LIGHTON);

	group.AddState(ALLOFFLASTBLINK, false);
	group.AddStateItem(ALLOFFLASTBLINK, red, LIGHTOFF);
	group.AddStateItem(ALLOFFLASTBLINK, orange, LIGHTOFF);
	group.AddStateItem(ALLOFFLASTBLINK, green, LIGHTOFF);
	group.AddStateItem(ALLOFFLASTBLINK, white, LIGHTBLINK);
	
	group.Toggle(ALLOFF);
}

void loop()
{
	Accessories::loop();

	Commanders::loop();
}
