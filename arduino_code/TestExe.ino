/*
	ShiftRegister74HC595 - Library for simplified control of 74HC595 shift registers.
	Created by Timo Denk (www.timodenk.com), Nov 2014.
	Additional information is available at http://shiftregister.simsso.de/
	Released into the public domain.
*/

#include "ShiftRegister74HC595.h"

// create a global shift register object
// parameters: (number of shift registers, data pin, clock pin, latch pin)
ShiftRegister74HC595 sr(1, 0, 1, 2);

void setup() {
}

void loop() {

	// setting all pins at the same time to either HIGH or LOW
	sr.setAllHigh(); // set all pins HIGH
	delay(1500);

	sr.setAllLow(); // set all pins LOW
	delay(1500);


	// setting single pins
	for (int i = 0; i < 8; i++) {

		sr.set(i, HIGH); // set single pin HIGH
		delay(500);
	}


	// set all pins at once
	uint8_t pinValues[] = { B10101010 };
	sr.setAll(pinValues);
	delay(1500);


	// read pin (zero based, i.e. 6th pin)
	uint8_t stateOfPin5 = sr.get(5);


	// set pins without immediate update
	sr.setNoUpdate(0, HIGH);
	sr.setNoUpdate(1, LOW);
	// at this point of time, pin 0 and 1 did not change yet
	sr.updateRegisters(); // update the pins to the set values
}
#if 0
/*************************************************************
digitalWrite.ino
SparkFun SX1509 I/O Expander Example: digital out (digitalWrite)
Jim Lindblom @ SparkFun Electronics
Original Creation Date: September 21, 2015
https://github.com/sparkfun/SparkFun_SX1509_Arduino_Library

This simple example demonstrates the SX1509's digital output
functionality. Attach an LED to SX1509 IO 15, or just look at
it with a multimeter. We're gonna blink it!

Hardware Hookup:
	SX1509 Breakout ------ Arduino -------- Breadboard
				GND -------------- GND
				3V3 -------------- 3.3V
			SDA ------------ SDA (A4)
			SCL ------------ SCL (A5)
			15 -------------------------------- LED+
																		 LED- -/\/\/\- GND
																								330

Development environment specifics:
	IDE: Arduino 1.6.5
	Hardware Platform: Arduino Uno
	SX1509 Breakout Version: v2.0

This code is beerware; if you see me (or any other SparkFun
employee) at the local, and you've found our code helpful,
please buy us a round!

Distributed as-is; no warranty is given.
*************************************************************/

#include <Wire.h> // Include the I2C library (required)
#include "SparkFunSX1509.h" // Include SX1509 library

// SX1509 I2C address (set by ADDR1 and ADDR0 (00 by default):
const byte SX1509_ADDRESS = 0x3E;  // SX1509 I2C address
const byte SX1509_ADDRESS2 = 0x3F;  // SX1509 I2C address
SX1509 io; // Create an SX1509 object to be used throughout
SX1509 io2; // Create an SX1509 object to be used throughout

// SX1509 pin definitions:
const byte SX1509_LED_PIN = 15; // LED connected to pin 15
const byte SX1509_LED_PIN2 = 8; // LED connected to pin 15

void setup()
{
	// Call io.begin(<address>) to initialize the SX1509. If it
	// successfully communicates, it'll return 1.
	if (!io.begin(SX1509_ADDRESS))
	{
		while (1); // If we fail to communicate, loop forever.
	}
	if (!io2.begin(SX1509_ADDRESS2))
	{
		while (1); // If we fail to communicate, loop forever.
	}

	// Call io.pinMode(<pin>, <mode>) to set an SX1509 pin as
	// an output:
	io.pinMode(SX1509_LED_PIN, OUTPUT);
	io2.pinMode(SX1509_LED_PIN2, OUTPUT);
}

void loop()
{
	// It's blinken time!
	// Call io.digitalWrite(<pin>, <HIGH | LOW>) to set a SX1509
	// output pin as either 3.3V or 0V.
	io.digitalWrite(SX1509_LED_PIN, HIGH);
	io2.digitalWrite(SX1509_LED_PIN2, LOW);
	delay(500); // Delay half-a-second
	io.digitalWrite(SX1509_LED_PIN, LOW); // Set the I/O low
	io2.digitalWrite(SX1509_LED_PIN2, HIGH);
	delay(500); // Delay half-a-second
}

/***********************************************************************************/

#include "Commanders.h"
#include "Accessories.h"

//------------------------------------------------------------------------------
// SignalArduino declaration

class SignalArduino : public AccessoryLightMulti
{
public:
	void begin(byte inNbLeds, int *inpPins, int inStartingDcc);
};

//------------------------------------------------------------------------------
// SignalArduino definition

static byte counter = 0;

void SignalArduino::begin(byte inNbLeds, int *inpPins, int inStartingDcc)
{
	this->AccessoryLightMulti::begin(1000 + (counter++), inNbLeds, 0);

	for (int led = 0; led < inNbLeds; led++)
	{
		PortOnePin *port = new PortOnePin();
		port->begin(inpPins[led], DIGITAL);
		this->beginLight(led, port);
	}

	// Used dcc codes are
	//                 Led  0    1    2
	// inStartingDcc   / 0  on  off  off
	// inStartingDcc   / 1  off on   off
	// inStartingDcc+1 / 0  off off  on
	// inStartingDcc+1 / 1  off off  off

	this->AdjustMovingPositionsSize(inNbLeds);

	int dcc = inStartingDcc;
	bool etat = false;
	for (int i = 0; i < this->GetSize(); i++)
	{
		if (!etat)
		{
			this->AddMovingPosition(DCCINT(dcc, 0), 1 << i, 0);
			etat = true;
		}
		else
		{
			this->AddMovingPosition(DCCINT(dcc, 1), 1 << i, 0);
			dcc++;
			etat = false;
		}
	}

	// Last moving position used to set all off.
	this->AddMovingPosition(DCCINT(dcc, etat == true ? 1 : 0), 0, 0);
}
// End of class
//------------------------------------------------------------------------------

/* kDCC_INTERRUPT values :
Board         int.0   int.1   int.2   int.3   int.4   int.5
Uno, Ethernet   2      3
Mega2560        2      3      21      20      19      18
Leonardo        3      2      0       1       7
*/
#define kDCC_INTERRUPT            0

#define NB_LEDS     3
#define NB_ETATS    3
#define NB_FEUX     16

int pins[][NB_LEDS] = {
	{ 5, 6, 7 },
	{ 8, 9, 10 },
	{ 11, 12, 13 },
	{ 14, 15, 16 },
	{ 17, 18, 19 },
	{ 20, 21, 22 },
	{ 23, 24, 25 },
	{ 26, 27, 28 },
	{ 29, 30, 31 },
	{ 32, 33, 34 },
	{ 35, 36, 37 },
	{ 38, 39, 40 },
	{ 41, 42, 43 },
	{ 44, 45, 46 },
	{ 47, 48, 49 },
	{ 50, 51, 52 }
};

ButtonsCommanderPush poussoir;
SignalArduino signaux[NB_FEUX];

int dcc_codes[] = { 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40 };

void setup()
{
	Commanders::begin();
	Accessories::begin();

	DccCommander.begin(0x00, 0x00, kDCC_INTERRUPT);

	poussoir.begin(0, A2);

	// Ce petit bouton va permettre de passer en revue tous les codes dcc des feux en séquence...
	int dcc = 0;
	for (int feu = 0; feu < NB_FEUX; feu++)
	{
		poussoir.AddEvent(DCCINT(dcc_codes[dcc], 0));
		poussoir.AddEvent(DCCINT(dcc_codes[dcc], 1));
		poussoir.AddEvent(DCCINT(dcc_codes[dcc] + 1, 0));
		dcc++;
	}

	for (int feu = 0; feu < NB_FEUX; feu++)
	{
		signaux[feu].begin(NB_LEDS, pins[feu], dcc_codes[feu]);
	}

	Accessories::printAccessories();
}

void loop()
{
	unsigned long id = Commanders::loop();

	if (id != UNDEFINED_ID)
	{
		// Renvoie l'événement reçu de Commanders, vers les accessoires...
		Accessories::RaiseEvent(id, (ACCESSORIES_EVENT_TYPE)Commanders::GetLastEventType(), Commanders::GetLastEventData());
	}

	Accessories::loop();
}
#endif
#if 0
/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <2 servos and 3 leds driven by dcc arduino>
*************************************************************/

#include <Accessories.h>
#include <Commanders.h>

ButtonsCommanderPush push;
ButtonsCommanderKeyboard greenButton, orangeButton, whiteButton;
ButtonsCommanderKeyboard servo1Button;
ButtonsCommanderKeyboard servo2_minButton, servo2_maxButton;
ButtonsCommanderKeyboard motorOneWayButton;
ButtonsCommanderKeyboard motorTwoWaysButton;
ButtonsCommanderKeyboard groupServos1Button;
ButtonsCommanderKeyboard groupServos2Button;
ButtonsCommanderKeyboard groupLights1Button;
ButtonsCommanderKeyboard groupLights2Button;
ButtonsCommanderSwitch swiitch;	// two 'ii' to avoid collision with the C word 'switch' !
ButtonsCommanderPotentiometer potar;

SERIAL_COMMANDER(Serial);

// Accessories

AccessoryServo servo1;
AccessoryServo servo2;
AccessoryLight green;
AccessoryLight orange;
AccessoryLight white;

AccessoryMotorOneWay motorOneWay;
AccessoryMotorTwoWays motorTwoWays;

AccessoryGroup groupServos;
AccessoryGroup groupLights;

// Ports

PortServo PortServo1;
PortServo PortServo2;

PortOnePin PortLightGreen;
PortOnePin PortLightOrange;
PortOnePin PortLightWhite;

PortOnePin PortMotorOneWay;
PortTwoPins PortMotorTwoWays;

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
	Commanders::begin(ReceiveEvent, LED_BUILTIN);
	DccCommander.begin(0, 0, 2);

	Accessories::begin(10, 1000);

	Serial.begin(115200);
	SerialCommander.begin();

	greenButton.begin(1000, '1');
	orangeButton.begin(1001, '2');
	whiteButton.begin(1002, '3');

	servo1Button.begin(2000, '0');

	servo2_minButton.begin(2100, '-');
	servo2_maxButton.begin(2101, '+');

	motorOneWayButton.begin(3000, '6');

	motorTwoWaysButton.begin(3100, '9');
	
	groupServos1Button.begin(5000, '4');
	groupServos2Button.begin(5001, '5');

	groupLights1Button.begin(5100, '7');
	groupLights2Button.begin(5101, '8');

	// The push button send two Dcc codes in turn, and is connected to pin 26.
	push.begin(1000, 26);	// id 1000, pin 26
	push.AddEvent(DCCINT(320, 0));
	push.AddEvent(DCCINT(320, 1));

	// The switch have two positions, each one controls one Dcc code. Each one connected to its pin.
	swiitch.begin();
	swiitch.AddEvent(DCCINT(319, 0), 24);
	swiitch.AddEvent(DCCINT(319, 1), 25);

	// The potentiometer is on analog pin 8.
	potar.begin(1001, 8, 0, 100);

	// Drivers setups

	// List of the ports on the Arduino. Pors 9,10 and 11 are handled in analog mode for fading.
	PortServo1.begin(2);
	PortServo2.begin(3);

	PortLightGreen.begin(9, ANALOG);
	PortLightOrange.begin(10, ANALOG);
	PortLightWhite.begin(11, ANALOG);

	PortMotorOneWay.begin(15, ANALOG);
	PortMotorTwoWays.begin(20, 21, ANALOG_INVERTED);

	// Accessories setups

	// Attach the servos to their driver/ports.
	// Servo1 can move from 30 to 145 degrees.
	servo1.begin(&PortServo1, 20, 30, 45);
	servo1.AddMovingPosition(2000, MINIMUM);
	servo1.AddMovingPosition(2001, MAXIMUM);

	// Servo1 has a pin 49 to control a relay giving power to the servo.
	servo1.SetPowerCommand(49);

	// Servo2 can move from 10 to 150 degrees. But there is also two more positions at 45 and 135 degrees,
	// commanded by Dcc codes 315/0 and 315/1.
	servo2.begin(&PortServo2, 0, 45, 55, 2);
	servo2.AddMovingPosition(2100, MINIMUM);
	servo2.AddMovingPosition(2101, MAXIMUM);

	// Attach the lights to their driver/ports.
	green.begin(&PortLightGreen, 1000);
	orange.begin(&PortLightOrange, 1001);
	white.begin(&PortLightWhite, 1002);

	// Declare light fading/dimming.
	green.SetFading(20, 10);
	orange.SetFading(20, 10);
	//white.SetFading(20, 10);

	motorOneWay.begin(&PortMotorOneWay, 3000, 200, 200);
	motorTwoWays.begin(&PortMotorTwoWays, 3100, 200, 200);

	groupLights.AddState(5100);
	groupLights.AddStateItem(5100, green, LIGHTON);
	groupLights.AddStateItem(5100, orange, LIGHTOFF);
	groupLights.AddStateItem(5100, white, LIGHTON);

	groupLights.AddState(5101);
	groupLights.AddStateItem(5101, green, LIGHTOFF);
	groupLights.AddStateItem(5101, orange, LIGHTON);
	groupLights.AddStateItem(5101, white, LIGHTOFF);

	groupServos.AddState(5000, true);
	groupServos.AddStateItem(5000, servo1, MINIMUM, 500);
	groupServos.AddStateItem(5000, servo2, MINIMUM, 500);

	groupServos.AddState(5001, true);
	groupServos.AddStateItem(5001, servo1, MAXIMUM, 500);
	groupServos.AddStateItem(5001, servo2, MAXIMUM, 500);

//	Commanders::printCommanders();

	Accessories::printAccessories();
}

void loop()
{
	Commanders::loop();
	Accessories::loop();
}
#endif

#if 0
#include <Accessories.h>
#include <Commanders.h>

// Les boutons...
ButtonsCommanderPush boutonDecoupleur;
ButtonsCommanderSwitch boutonAiguillageGauche;
ButtonsCommanderSwitch boutonAiguillageDroite;

// Les trois moteurs
AccessoryMotorTwoWays aiguillageGauche;
AccessoryMotorTwoWays aiguillageDroite;
AccessoryMotorOneWay decouplage;

// Les ports pour connecter les moteurs.
PortTwoPins portAiguillageGauche;
PortTwoPins portAiguillageDroite;
PortOnePin portDecoupleur;

SERIAL_COMMANDER(Serial);

void setup()
{
	Serial.begin(115200);

	Commanders::begin();
	Accessories::begin();

	// Activation de la reception de messages Dcc
	// Ne fera rien en analogique... Mais on peut carrement enlever la ligne si besoin de mémoire.
	DccCommander.begin(0x00, 0x00, digitalPinToInterrupt(3));
	SerialCommander.begin();

	// Gestion des boutons pour le digital ou le numérique.
	// Ne fera rien si aucun bouton n'est branché, mais on peut enlever si besoin de mémoire.
	//
	// Les boutons doivent décrire quel identifiant est concerné s'ils sont activés.
	// Un switch peut avoir une, deux ou beaucoup plus de positions.
	// Ici, seules deux positions sont déclarées avec pour chacune une broche associée, et un identifiant émis.
	boutonAiguillageGauche.begin();
	boutonAiguillageGauche.AddEvent(DCCINT(15, 0), 2);
	boutonAiguillageGauche.AddEvent(DCCINT(15, 1), 7);

	boutonAiguillageDroite.begin();
	boutonAiguillageDroite.AddEvent(DCCINT(16, 0), 4);
	boutonAiguillageDroite.AddEvent(DCCINT(16, 1), 5);

	boutonDecoupleur.begin(DCCINT(17, 0), 6);

	// Les ports avec leurs broches en digital (pas PWM)
	portAiguillageGauche.begin(8, 9, DIGITAL);
	portAiguillageGauche.begin(10, 11, DIGITAL);
	portDecoupleur.begin(12, DIGITAL);

	// Les accessoires avec pour chaque, le port utilisé et le ou les identifiants associés à chaque position. 
	// 255 est la vitesse, ici au maxi, et 400 est la durée d'activation du moteur en millisecondes.
	// C'est encore un peu long pour des solénoïdes. Il faudrait adapter selon le modèle de moteur...
	aiguillageGauche.beginTwoWays(&portAiguillageGauche, DCCINT(15, 0), DCCINT(15, 1), 255, 400);
	aiguillageDroite.beginTwoWays(&portAiguillageDroite, DCCINT(16, 0), DCCINT(16, 1), 255, 400);
	decouplage.begin(&portDecoupleur, DCCINT(17, 0), 255, 400);
}

void loop()
{
	unsigned long id = Commanders::loop();

	if (id != UNDEFINED_ID)
	{
		// Renvoie l'événement reçu de Commanders, vers les accessoires...
		Accessories::RaiseEvent(id, (ACCESSORIES_EVENT_TYPE) Commanders::GetLastEventType(), Commanders::GetLastEventData());
	}

	Accessories::loop();
}
#endif

#if 0
#include <Commanders.h>

void setup()
{

	Serial.begin(115200);

	Commanders::begin(LED_BUILTIN);
	DccCommander.begin(0x00, 0x00, digitalPinToInterrupt(3));
}

void loop()
{
	unsigned long ret = Commanders::loop();

	if (ret != UNDEFINED_ID)
	{
		Serial.print(DCCID(ret));
		Serial.print("   ");
		Serial.print(DCCACTIVATION(ret));
		Serial.println("     ");
	}
}
#endif