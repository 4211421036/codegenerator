/*************************************************************
project: <Carte 8 Servos Locoduino by Accessories>
author: <Thierry PARIS>
*************************************************************/

#include <Commanders.h>
#include <Accessories.h>

#ifdef NO_CANCOMMANDER
#error To be able to compile this sample,the line #define NO_CANCOMMANDER must be commented in Commanders.h
#endif

#define SERVO0_MIN	DCCINT(20, 0)
#define SERVO0_MAX	DCCINT(20, 1)
#define SERVO1_MIN	DCCINT(21, 0)
#define SERVO1_MAX	DCCINT(21, 1)
#define SERVO2_MIN	DCCINT(22, 0)
#define SERVO2_MAX	DCCINT(22, 1)
#define SERVO3_MIN	DCCINT(23, 0)
#define SERVO3_MAX	DCCINT(23, 1)
#define SERVO4_MIN	DCCINT(24, 0)
#define SERVO4_MAX	DCCINT(24, 1)
#define SERVO5_MIN	DCCINT(25, 0)
#define SERVO5_MAX	DCCINT(25, 1)
#define SERVO6_MIN	DCCINT(26, 0)
#define SERVO6_MAX	DCCINT(26, 1)
#define SERVO7_MIN	DCCINT(27, 0)
#define SERVO7_MAX	DCCINT(27, 1)

// Commanders

SERIAL_COMMANDER(Serial);

// Drivers

PortServo Port0;
PortServo Port1;
PortServo Port2;
PortServo Port3;
PortServo Port4;
PortServo Port5;
PortServo Port6;
PortServo Port7;

// current user define speed.
int speed;

// servo accessory.
AccessoryServo servo0;
AccessoryServo servo1;
AccessoryServo servo2;
AccessoryServo servo3;
AccessoryServo servo4;
AccessoryServo servo5;
AccessoryServo servo6;
AccessoryServo servo7;

void ReceiveEvent(unsigned long inId, COMMANDERS_EVENT_TYPE inEventType, int inEventData)
{
	Accessories::ReceiveEvent(inId, (ACCESSORIES_EVENT_TYPE) inEventType, inEventData);
	
	#ifdef ACCESSORIES_DEBUG_MODE
    COMMANDERS_PRINT_EVENT(inId, inEventType, inEventData);
    #endif
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

	DccCommander.begin(0x00, 0x00, digitalPinToInterrupt(2));
	SerialCommander.begin();

	CANCommander.begin(10, CAN_125KBPS, digitalPinToInterrupt(3), 100);

	// Ports setups

	Port0.begin(6);
	Port1.begin(7);
	Port2.begin(8);
	Port3.begin(9);
	Port4.begin(A0);
	Port5.begin(A1);
	Port6.begin(A2);
	Port7.begin(A3);

	// Accessories setups

	speed = 19;	// starting speed state : fast.

	servo0.begin(&Port0, speed, 0, 10, 2);
	servo1.begin(&Port1, speed, 0, 10, 2);
	servo2.begin(&Port2, speed, 0, 10, 2);
	servo3.begin(&Port3, speed, 0, 10, 2);
	servo4.begin(&Port4, speed, 0, 10, 2);
	servo5.begin(&Port5, speed, 0, 10, 2);
	servo6.begin(&Port6, speed, 0, 10, 2);
	servo7.begin(&Port7, speed, 0, 10, 2);

	servo0.AddMinMaxMovingPositions(SERVO0_MIN, SERVO0_MAX);
	servo1.AddMinMaxMovingPositions(SERVO1_MIN, SERVO1_MAX);
	servo2.AddMinMaxMovingPositions(SERVO2_MIN, SERVO2_MAX);
	servo3.AddMinMaxMovingPositions(SERVO3_MIN, SERVO3_MAX);
	servo4.AddMinMaxMovingPositions(SERVO4_MIN, SERVO4_MAX);
	servo5.AddMinMaxMovingPositions(SERVO5_MIN, SERVO5_MAX);
	servo6.AddMinMaxMovingPositions(SERVO6_MIN, SERVO6_MAX);
	servo7.AddMinMaxMovingPositions(SERVO7_MIN, SERVO7_MAX);
}

void loop()
{
	Commanders::loop();
	Accessories::loop();
}
