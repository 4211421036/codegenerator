/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <Test for Accessories library, with some turnouts and lights, controled by Dcc.>
*************************************************************/

#include "Commanders.h"
#include "Accessories.h"

#define IDLEFT		DCCINT(20, 0)
#define IDRIGHT		DCCINT(20, 1)
#define IDDC		DCCINT(21, 0)
#define IDEPS		DCCINT(21, 1)
#define IDTJD		DCCINT(22, 0)
#define IDSERVOMIN	DCCINT(22, 1)
#define IDSERVOMAX	DCCINT(23, 0)
#define IDLIGHT		DCCINT(23, 1)

// Commanders

SERIAL_COMMANDER(Serial);

#ifdef VISUALSTUDIO
ButtonsCommanderKeyboard push0;
ButtonsCommanderKeyboard push1;
ButtonsCommanderKeyboard push2;
ButtonsCommanderKeyboard push3;
ButtonsCommanderKeyboard push4;
ButtonsCommanderKeyboard push5;
ButtonsCommanderKeyboard push6;
ButtonsCommanderKeyboard push7;
#else
ButtonsCommanderPush push0;
ButtonsCommanderPush push1;
ButtonsCommanderPush push2;
ButtonsCommanderPush push3;
ButtonsCommanderPush push4;
ButtonsCommanderPush push5;
ButtonsCommanderPush push6;
ButtonsCommanderPush push7;
#endif

// Accessories

AccessoryMotorTwoWays turnoutLeft;
AccessoryMotorTwoWays turnoutRight;
AccessoryMotorTwoWays turnoutDc;
AccessoryMotorTwoWays turnoutEps;
AccessoryMotorTwoWays turnoutTjd;
AccessoryServo servo;
AccessoryLight light;

// Ports

// l293 part
PortShieldL293d PortLeft;
PortShieldL293d PortRight;
PortShieldL293d PortDc;
PortShieldL293d PortEps;
PortServo portServo;

// The double turnout is on the l298n circuit, with the led.
PortTwoPins PortTjd;
PortTwoPins PortLight;

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

	// Each button assigned to an accessory Dcc code.
#ifdef VISUALSTUDIO
	push0.begin(IDLEFT, '0');
	push1.begin(IDRIGHT, '1');
	push2.begin(IDDC, '2');
	push3.begin(IDEPS, '3');
	push4.begin(IDTJD, '4');
	push5.begin(IDSERVOMIN, '5');
	push6.begin(IDSERVOMAX, '6');
	push7.begin(IDLIGHT, '7');
#else
	push0.begin(IDLEFT, 30);
	push1.begin(IDRIGHT, 32);
	push2.begin(IDDC, 34);
	push3.begin(IDEPS, 36);
	push4.begin(IDTJD, 38);
	push5.begin(IDSERVOMIN, 40);
	push6.begin(IDSERVOMAX, 42);
	push7.begin(IDLIGHT, 44);
#endif
	// Drivers setups

	// four turnouts are connected to the l293d shield.
	// l293d part
	PortLeft.begin(SHIELDL293D_PORT_M1, 200, MOTOR12_1KHZ);
	PortRight.begin(SHIELDL293D_PORT_M2, 200, MOTOR12_1KHZ);
	PortDc.begin(SHIELDL293D_PORT_M3, 200, MOTOR34_1KHZ);
	PortEps.begin(SHIELDL293D_PORT_M4, 200, MOTOR34_1KHZ);
	portServo.begin(SHIELDL293D_SERVO1_PIN);

	// l298n circuit.
	PortTjd.begin(50, 52, DIGITAL);
	PortLight.begin(46, 48, DIGITAL);

	// Accessories setups

	// Assign Dcc code for each accessory.
	turnoutLeft.begin(&PortLeft, IDLEFT, 50, 150);
	turnoutRight.begin(&PortRight, IDRIGHT, 3000, 150);
	turnoutDc.begin(&PortDc, IDDC, 500, 150);
	turnoutEps.begin(&PortEps, IDEPS, 3000, 150);

	turnoutTjd.begin(&PortTjd, IDTJD, 0, 150);
	light.begin(&PortLight, IDLIGHT, 500, 150);
	servo.begin(&portServo, 1000, 40, 50, 2);
	servo.AddMinMaxMovingPositions(IDSERVOMIN, IDSERVOMAX);
}

void loop()
{
	Accessories::loop();

	Commanders::loop();
}
