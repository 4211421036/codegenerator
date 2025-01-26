/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <2 servos and 3 leds driven by arduino>
*************************************************************/

#include "Commanders.h"
#include "Accessories.h"

// DCC codes
#define SERVO0		DCCINT(20, 0)
#define SERVO1_MIN	DCCINT(21, 0)
#define SERVO1_MAX	DCCINT(21, 1)
#define SERVO1_45	DCCINT(22, 0)
#define SERVO1_55	DCCINT(22, 1)

#define SERVOS_MIN	DCCINT(25, 0)
#define SERVOS_MAX	DCCINT(25, 1)

#define LIGHTS_010	DCCINT(26, 0)
#define LIGHTS_101	DCCINT(26, 1)

// Commanders

SERIAL_COMMANDER(Serial);

#ifdef VISUALSTUDIO
ButtonsCommanderKeyboard	push;
ButtonsCommanderKeyboard	switch0;
ButtonsCommanderKeyboard	switch1;
#else
ButtonsCommanderPush push;
ButtonsCommanderSwitch	switch0, switch1;
#endif

ButtonsCommanderPotentiometer potar;

// Accessories

AccessoryServo servo0, servo1;
AccessoryLight light0, light1, light2;

AccessoryGroup groupServos;
AccessoryGroup groupLights;

// List of the ports on the Arduino. Light Ports are handled in analog mode for fading.
PortServo PortServo0;
PortServo PortServo1;
PortOnePin PortLight0;
PortOnePin PortLight1;
PortOnePin PortLight2;

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
//	while (!Serial);		// For Leonardo only. No effect on other Arduino.

	Commanders::begin(ReceiveEvent, LED_BUILTIN);
	Accessories::begin();

	// Commanders setup

	SerialCommander.begin();
	DccCommander.begin(0x00, 0x00, digitalPinToInterrupt(3));

#ifdef VISUALSTUDIO
	push.begin(SERVOS_MIN, '0');
	switch0.begin(LIGHTS_010, '1');
	switch1.begin(LIGHTS_101, '2');
#else
	push.begin(SERVOS_MIN, 26);
	switch0.begin();
	switch0.AddEvent(24, LIGHTS_010);
	switch1.begin();
	switch1.AddEvent(26, LIGHTS_101);
#endif
	push.AddEvent(SERVOS_MAX);
	potar.begin(8, SERVO0, 20, 145);

	// Ports setups

	PortServo0.begin(2);
	PortServo1.begin(3);
	PortLight0.begin(9, ANALOG);
	PortLight1.begin(10, ANALOG);
	PortLight2.begin(11, ANALOG);

	// Accessories setups

    // Declare accessories.
	servo0.begin(&PortServo0, 20, 20, 30, 1);
	servo0.AddMovingPosition(SERVO0, UNDEFINED_POS);
	servo0.SetPowerCommand(49);

	servo1.begin(&PortServo1, 20, 10, 40, 4);
	servo1.AddMinMaxMovingPositions(SERVO1_MIN, SERVO1_MAX);
	servo1.AddMovingPosition(SERVO1_45, 20);
	servo1.AddMovingPosition(SERVO1_55, 30);

	light0.begin(&PortLight0, 1);
	light1.begin(&PortLight1, 2);
	light2.begin(&PortLight2, 3);
	
    // Servo1 has a pin 49 to control a relay giving power to the servo.

    // Declare light fading/dimming.
	light0.SetFading(20, 10);
	light1.SetFading(20, 10);
	//light2.SetFading(20, 10);

	groupServos.begin();
	groupServos.AddState(SERVOS_MIN);
	groupServos.AddStateItem(SERVOS_MIN, servo0, MINIMUM, 500);
	groupServos.AddStateItem(SERVOS_MIN, servo1, MINIMUM, 500);

	groupServos.AddState(SERVOS_MAX);
	groupServos.AddStateItem(SERVOS_MAX, servo0, MAXIMUM, 500);
	groupServos.AddStateItem(SERVOS_MAX, servo1, MAXIMUM, 500);

	groupLights.begin();
	groupLights.AddState(LIGHTS_010);
	groupLights.AddStateItem(LIGHTS_010, light0, LIGHTON);
	groupLights.AddStateItem(LIGHTS_010, light1, LIGHTOFF);
	groupLights.AddStateItem(LIGHTS_010, light2, LIGHTON);

	groupLights.AddState(LIGHTS_101);
	groupLights.AddStateItem(LIGHTS_101, light0, LIGHTOFF);
	groupLights.AddStateItem(LIGHTS_101, light1, LIGHTON);
	groupLights.AddStateItem(LIGHTS_101, light2, LIGHTOFF);
}

void loop()
{
	Accessories::loop();

	Commanders::loop();
}
