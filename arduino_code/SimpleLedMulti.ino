/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <Test for Accessories library, with five leds, controled by Dcc.>
*************************************************************/

#include "Commanders.h"
#include "Accessories.h"
#include "binary.h"

// DCC codes
#define SIGNAL0		DCCINT(20,0)
#define SIGNAL1		DCCINT(21,0)
#define SIGNAL2		DCCINT(22,0)
#define SIGNAL3		DCCINT(23,0)
#define SIGNAL4		DCCINT(24,0)
#define SIGNAL5		DCCINT(25,0)
#define SIGNAL6		DCCINT(26,0)
#define SIGNAL7		DCCINT(27,0)
#define SIGNAL8		DCCINT(28,0)
#define SIGNAL9		DCCINT(29,0)

// Commanders

SERIAL_COMMANDER(Serial);

// Accessories

AccessoryLightMulti Signal;
																	   
// Ports

PortOnePin Port0;
PortOnePin Port1;
PortOnePin Port2;
PortOnePin Port3;
PortOnePin Port4;



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

	// Ports setups

	// one light is connected to the arduino.
	Port0.begin(4, DIGITAL);
	Port1.begin(5, DIGITAL);
	Port2.begin(6, DIGITAL);
	Port3.begin(7, DIGITAL);
	Port4.begin(8, DIGITAL);

	// Accessories setups

	Signal.begin(0, 5, 500); // id not significant here

	// Attach each light to its driver/port.
	Signal.beginLight(0, &Port0);
	Signal.beginLight(1, &Port1);
	Signal.beginLight(2, &Port2);
	Signal.beginLight(3, &Port3);
	Signal.beginLight(4, &Port4);

	Signal.AdjustMovingPositionsSize(10);

	Signal.AddMovingPosition(SIGNAL0, B10000, B10000);
	Signal.AddMovingPosition(SIGNAL1, B01000, B01000);
	Signal.AddMovingPosition(SIGNAL2, B00100, B00100);
	Signal.AddMovingPosition(SIGNAL3, B00010, B00010);
	Signal.AddMovingPosition(SIGNAL4, B00001, B00001);
	Signal.AddMovingPosition(SIGNAL5, B10001, B10000);
	Signal.AddMovingPosition(SIGNAL6, B01001, B01000);
	Signal.AddMovingPosition(SIGNAL7, B00101, B00100);
	Signal.AddMovingPosition(SIGNAL8, B00011, B00010);
	Signal.AddMovingPosition(SIGNAL9, B00101, B00001);

	/*
	// Test code to pass through all the states during 5 seconds...
	for (int i = 0; i < Signal.GetMovingPositionSize(); i++)
	{
		Accessories::RaiseEvent(DCCINT(20 + i, 0));

		unsigned long start = millis();

		while (millis() - start < 5000)
			Accessories::loop();
	}
	*/
	Signal.LightOff();
}

void loop()
{
	Accessories::loop();

	Commanders::loop();
}