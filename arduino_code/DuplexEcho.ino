/*
  $Id: DuplexEcho.ino,v 1.6 2018/03/07 12:40:42 ewan Exp $
  Echo serial i/o between the built in USART and the external MAX3100.
  DuplexEcho.ino (C) 2016 Ewan Parker.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

  The author may be reached at https://www.ewan.cc/ for queries.


  Assemble the Arduino and MAX3100CPD circuit as follows.  This could be made
  to work with the other MAX3100 packages with minor changes to the wiring.

Arduino                  MAX3100CPD
=======               ================             CH340/FTDI
MOSI 11 ----------> 1 DIN       Vcc 14 --- 5V      ==========
MISO 12 <---------- 2 DOUT       TX 13 ----------> RX
SCK  13 ----------> 3 SCLK       RX 12 <---------- TX
     10 ----------> 4 /CS      /RTS 11     GND --- GND
      2 <---------- 5 /IRQ     /CTS 10
             5V --- 6 /SHDN      X1  9 ----------- Crystal pin1 --- 22pF --- GND
            GND --- 7 GND        X2  8 ----------- Crystal pin2 --- 22pF --- GND

The crystal chosen above is 1.8432 MHz.  A 3.6864 MHz crystal may also be used.
In that case, set xtalFreq=3686 below.
*/

const byte csPin = 10;           // Chip select pin of Arduino
const int xtalFreqkHz = 1843;    // Chosen crystal frequency in kHz
const uint32_t internalBaud = 115200; // Baud rate to use for the Arduino USART
const uint32_t externalBaud = 115200; // Baud rate to use for the MAX3100 USART

#include <MAX3100Serial.h>
#include <SPI.h>

// Initialise MAX3100 instance with 1.8432 MHz crystal and chip select pin 10.
MAX3100Serial serial2 = MAX3100Serial(xtalFreqkHz, csPin);
// Flags denoting whether the interrupt was from the internal and/or external
// USART.
volatile boolean internal = false, external = false;

// For an Arduino serial event, set the appropriate flag.
void serialEvent()
{
  internal = true;
}

// For a MAX3100 serial event, set the appropriate flag.  Detach the interrupt
// otherwise it will keep firing before we've even emptied one character from
// the MAX3100 buffer.
void max3100_irq()
{
  detachInterrupt(0);
  external = true;
}

// Run this once at power on or reset.
void setup(void)
{
  // Arduino USART setup.
  Serial.begin(internalBaud);

  // Display USART header on Arduino.
  Serial.println();
  Serial.println("Maxim Integrated MAX3100 testing.");
  Serial.println("(C) 2016 Ewan Parker.");
  Serial.print(">>> Internal Arduino USART :: ");
  Serial.print(internalBaud); Serial.println(" bps <<<");
  Serial.println();

  // MAX3100 USART setup.
  serial2.begin(externalBaud);
  // Disable MAX3100 interrupt processing while in SPI communications.
  SPI.usingInterrupt(0);
  // Attach an interrupt so we know when we have received input from the
  // external USART.  Without this we would need to continually poll to see if
  // a byte was available.
  attachInterrupt(0, max3100_irq, LOW);

  // Display USART header on MAX3100.
  serial2.println("Maxim Integrated MAX3100 testing.");
  serial2.println("(C) 2016 Ewan Parker.");
  serial2.print(">>> External MAX3100CPD USART :: ");
  serial2.print(externalBaud); serial2.println(" bps <<<");
  serial2.println();
}

// Process this loop whenever we see a serial event or interrupt from the
// MAX3100.
void loop(void)
{
  // Incoming character from the Arduino's USART.
  if (internal)
  {
    internal = false;
    char c = Serial.read(); // Read the character from the Arduino
    serial2.write(c);       // and write it to the MAX3100
  }

  // Incoming character from the MAX3100's USART.
  if (external)
  {
    external = false;
    if (serial2.available())        // If there is a character from the MAX3100
      Serial.write(serial2.read()); // write it to the Arduino
    attachInterrupt(0, max3100_irq, LOW); // Check for further serial input
  }
}
