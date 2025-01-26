
#define __PROG__ "jm_LiquidCrystal_I2C_demo"

////////////////////////////////////////////////////////////////////////////////

#include <jm_Scheduler.h>

////////////////////////////////////////////////////////////////////////////////

#include <jm_Wire.h>
extern uint16_t twi_readFrom_timeout;
extern uint16_t twi_writeTo_timeout;
extern bool twi_readFrom_wait;
extern bool twi_writeTo_wait;

////////////////////////////////////////////////////////////////////////////////

#include <jm_LiquidCrystal_I2C.h>

jm_LiquidCrystal_I2C lcd;

const uint8_t charBitmap[8][8] = {
   { 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F }
};

////////////////////////////////////////////////////////////////////////////////

jm_Scheduler lcd_demo_scheduler;

void lcd_demo_coroutine()
{
	static long count = -1;
	Serial.println(++count);

	int32_t rand32 = -2147483648+random()+random();
	uint32_t time32 = millis();
	static char c = 0x00;

#if 0
	lcd.set_cursor( 0, 0 );
	lcd.print_s32( rand32 ); // 10+1 chars
	lcd.print_space();
	lcd.print_h32( (uint32_t)rand32 ); // 8 chars
	while (lcd._i2cio.yield_request()) jm_Scheduler::yield();

	lcd.set_cursor( 0, 1 );
	lcd.print_u32( time32 ); // 10 chars
	lcd.print_space();
	lcd.print_h32( time32 ); // 8 chars
	while (lcd._i2cio.yield_request()) jm_Scheduler::yield();
#else
	lcd.set_cursor( 0, 0 );
	lcd.print_space();
	lcd.print_s32( rand32 ); // 10+1 chars
	while (lcd._i2cio.yield_request()) jm_Scheduler::yield();

	lcd.set_cursor( 0, 1 );
//	lcd.print_u32f( time32, 4 ); // 10+1 chars
//	lcd.print_u32f( rand32, 9 ); // 10+1 chars
	lcd.print_s32f( rand32, 9 ); // 10+1+1 chars
	while (lcd._i2cio.yield_request()) jm_Scheduler::yield();
#endif

#if 0
	lcd.set_cursor( 0, 2 );
	lcd.print_h8( c );
	lcd.print_space();
	for (int i=0; i<0x10; i++) lcd.print( (char)(c + i) );
	while (lcd._i2cio.yield_request()) jm_Scheduler::yield();
	c += 0x10;
#else
	lcd.set_cursor( 0, 2 );
	lcd.print_u32f( time32, 3 ); // 10+1 chars
	while (lcd._i2cio.yield_request()) jm_Scheduler::yield();
#endif

	lcd.set_cursor( 0, 3 );
	lcd.print_h8( c );
	lcd.print_space();
	for (int i=0; i<0x10; i++) lcd.print( (char)(c + i) );
	while (lcd._i2cio.yield_request()) jm_Scheduler::yield();
	c += 0x10;
}

////////////////////////////////////////////////////////////////////////////////

void setup()
{
	Serial.begin(115200);
	while (!Serial && millis()<3000); // wait for USB Serial ready
	Serial.print(F(__PROG__));
	Serial.print(F("..."));
	Serial.println();

	Wire.begin();
	twi_readFrom_wait = false; // Suppress twi_readFrom() waiting loop
	twi_writeTo_wait = false; // Suppress twi_writeTo() waiting loop

	lcd.begin();
	while (lcd._i2cio.yield_request()) jm_Scheduler::yield();

	for( int i=0; i<8; i++ )
	{
		lcd.createChar( i, (uint8_t *)charBitmap[i] );
		while (lcd._i2cio.yield_request()) jm_Scheduler::yield();
	}

	lcd_demo_scheduler.start(lcd_demo_coroutine, jm_Scheduler_time_read() + 1*TIMESTAMP_1SEC, 1*TIMESTAMP_1SEC);
}

void loop()
{
	jm_Scheduler::yield();
}
