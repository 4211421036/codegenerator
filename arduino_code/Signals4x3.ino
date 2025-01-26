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
#define NB_FEUX     4

int pins[][NB_LEDS] = {
	{ 5, 6, 7 },
	{ 8, 9, 10 },
	{ 11, 12, 13 },
	{ 14, 15, 16 }
};

ButtonsCommanderPush poussoir;
SignalArduino* signaux[NB_FEUX];

int dcc_codes[] = { 10, 12, 14, 16 };

void setup()
{
	Commanders::begin();
	Accessories::begin();

	DccCommander.begin(0x00, 0x00, kDCC_INTERRUPT);

	poussoir.begin(0, A2);

	// Ce petit bouton va permettre de passer en revue tous les codes dcc des feux en séquence...
	for (int feu = 0; feu < NB_FEUX; feu++)
	{
		poussoir.AddEvent(DCCINT(dcc_codes[feu], 0));
		poussoir.AddEvent(DCCINT(dcc_codes[feu], 1));
		poussoir.AddEvent(DCCINT(dcc_codes[feu] + 1, 0));
	}

	for (int feu = 0; feu < NB_FEUX; feu++)
	{
		signaux[feu] = new SignalArduino();
		signaux[feu]->begin(NB_LEDS, pins[feu], dcc_codes[feu]);
	}
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
