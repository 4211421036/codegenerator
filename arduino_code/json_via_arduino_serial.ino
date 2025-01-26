/*
 * Copyright (C) 2015, 2018 Eugene Hutorny <eugene@hutorny.in.ua>
 *
 * json_via_arduino_serial.cpp - example for using cojson with Arduiono Serial
 *
 * This file is part of COJSON Library. http://hutorny.in.ua/projects/cojson
 *
 * The COJSON Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License v2
 * as published by the Free Software Foundation;
 *
 * The COJSON Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the COJSON Library; if not, see
 * <http://www.gnu.org/licenses/gpl-2.0.html>.
 */
#include "Arduino.h"
#include "cojson.h"


namespace MyApp {
    namespace name {
#		ifdef __AVR__
#		define CSTRING_NAME(a,b) inline cojson::cstring a() noexcept {\
                static const char l[] __attribute__((progmem))= b;\
                return cojson::cstring(l);\
            }
#		else
#		define CSTRING_NAME(a,b) inline constexpr cojson::cstring a() noexcept{\
                return b;\
           }
#		endif
        CSTRING_NAME(led,"led")
        CSTRING_NAME(blink,"blink")
        CSTRING_NAME(Int,"int")
        CSTRING_NAME(msg,"msg")
#		undef CSTRING_NAME
    }

    struct MyClass {
        bool led_get() const noexcept {
        	return digitalRead(LED_BUILTIN);
        }
        void led_set(bool value) noexcept {
        	digitalWrite(LED_BUILTIN, value & 1);
        }
        bool blink;
        int Int;
        char msg[32];
    public:
        static const cojson::details::clas<MyClass>& json() noexcept;
        inline bool read(cojson::details::lexer& in) noexcept {
            return json().read(*this, in);
        }
        inline bool write(cojson::details::ostream& out) const noexcept {
            return json().write(*this, out);
        }
    };

 /**************************************************************************/

    const cojson::details::clas<MyClass>& MyClass::json() noexcept {
        using namespace cojson;
        using namespace cojson::accessor;

        return
            O<MyClass,
            	P<MyClass, name::led, methods<
                	MyClass, bool, &MyClass::led_get, &MyClass::led_set>>,
                P<MyClass, name::blink, bool, &MyClass::blink>,
                P<MyClass, name::Int, int, &MyClass::Int>,
                P<MyClass, name::msg, 32, &MyClass::msg>
            >();
    }

 /**************************************************************************/
}

MyApp::MyClass control { true, 10, "Hello JSON"};

void setup() {
  cojson::wrapper::iostream<decltype(Serial)> serialio(Serial);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial) {};
  Serial.println("Arduino cojson example");
  control.write(serialio);
  Serial.print("\n:");
}


void blink(bool enable) {
	static constexpr int period = 1000;
	static auto last = millis();
	static bool toggle = false;
	if( millis() - last > period ) {
		last = millis();
		if( enable )
			digitalWrite(LED_BUILTIN, toggle);
		Serial.print((toggle ^= 1) ? "\r." : "\r:");
	}
}

void loop() {
	cojson::wrapper::iostream<decltype(Serial)> serialio(Serial);
    serialio.echo(true);
	while( !Serial.available() ) {
		blink(control.blink);
	}
	if( Serial.peek() == '\r' )
		Serial.read();
	else
		control.read(serialio);
	Serial.println();
	if( + serialio.error() ) {
		Serial.println("JSON error");
		serialio.clear();
		while( Serial.available() ) Serial.read();
	}
	control.write(serialio);
	Serial.println();
}
