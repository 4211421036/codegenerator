/************************************************************************

OLED_example_Adafruit

This is an example for the Monochrome OLED of DFRobot Accessory Shield
based on SSD1306 driver.
The OLED display is controlled through the Oled library that is based on
the Adafruit_SSD1306 library written by Limor Fried/Ladyada.
A special thanks to the Adafruit team for its great contibution regarding
the development of open source code

sketch based on the ssd1306_oled_128x64_i2c sketch written by Adafruit
modified 6 Sept 2016
by Biagio Montaruli
 
this code is in the public domain
*************************************************************************/

#include <AccessoryShield.h>

#if defined(__AVR_ATmega32U4__)
    #error "Arduino Leonardo pinout does not allow using OLED of Acessory Shield"
#endif

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Oled.h in src directory of AccessoryShield library");
#endif

void setup()   {                
  Serial.begin(9600);
  // initialize the AccessoryShield library and the Oled library
  accessoryShield.begin();
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  accessoryShield.oledPaint();
  delay(2000);

  // Clear the buffer.
  accessoryShield.clearOledDisplay();

  // draw a single pixel
  accessoryShield.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!
  accessoryShield.oledPaint();
  delay(2000);
  accessoryShield.clearOledDisplay();

  // draw many lines
  testdrawline();
  accessoryShield.oledPaint();
  delay(2000);
  accessoryShield.clearOledDisplay();

  // draw rectangles
  testdrawrect();
  accessoryShield.oledPaint();
  delay(2000);
  accessoryShield.clearOledDisplay();

  // draw multiple rectangles
  testfillrect();
  accessoryShield.oledPaint();
  delay(2000);
  accessoryShield.clearOledDisplay();

  // draw mulitple circles
  testdrawcircle();
  accessoryShield.oledPaint();
  delay(2000);
  accessoryShield.clearOledDisplay();

  // draw a white circle, 10 pixel radius
  accessoryShield.fillCircle(accessoryShield.width()/2, accessoryShield.height()/2, 10, WHITE);
  accessoryShield.oledPaint();
  delay(2000);
  accessoryShield.clearOledDisplay();

  testdrawroundrect();
  delay(2000);
  accessoryShield.clearOledDisplay();

  testfillroundrect();
  delay(2000);
  accessoryShield.clearOledDisplay();

  testdrawtriangle();
  delay(2000);
  accessoryShield.clearOledDisplay();
   
  testfilltriangle();
  delay(2000);
  accessoryShield.clearOledDisplay();

  // draw the first ~12 characters in the font
  testdrawchar();
  accessoryShield.oledPaint();
  delay(2000);
  accessoryShield.clearOledDisplay();

  // draw scrolling text
  testscrolltext();
  delay(2000);
  accessoryShield.clearOledDisplay();

  // text display tests
  accessoryShield.setTextSize(1);
  accessoryShield.setTextColor(WHITE);
  accessoryShield.setCursor(0,0);
  accessoryShield.println("Hello, world!");
  accessoryShield.setTextColor(BLACK, WHITE); // 'inverted' text
  accessoryShield.println(3.141592);
  accessoryShield.setTextSize(2);
  accessoryShield.setTextColor(WHITE);
  accessoryShield.print("0x");
  accessoryShield.println(0xDEADBEEF, HEX);
  accessoryShield.oledPaint();
  delay(2000);
  accessoryShield.clearOledDisplay();

  // miniature bitmap display
  accessoryShield.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  accessoryShield.oledPaint();
  delay(1);

  // invert the display
  accessoryShield.invertOledDisplay(true);
  delay(1000); 
  accessoryShield.invertOledDisplay(false);
  delay(1000); 
  accessoryShield.clearOledDisplay();

  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}


void loop() {
  
}


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(accessoryShield.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
    
    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      accessoryShield.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }
    accessoryShield.oledPaint();
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      accessoryShield.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > accessoryShield.height()) {
        icons[f][XPOS] = random(accessoryShield.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
}


void testdrawchar(void) {
  accessoryShield.setTextSize(1);
  accessoryShield.setTextColor(WHITE);
  accessoryShield.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    accessoryShield.write(i);
    if ((i > 0) && (i % 21 == 0))
      accessoryShield.println();
  }    
  accessoryShield.oledPaint();
  delay(1);
}

void testdrawcircle(void) {
  for (int16_t i=0; i<accessoryShield.height(); i+=2) {
    accessoryShield.drawCircle(accessoryShield.width()/2, accessoryShield.height()/2, i, WHITE);
    accessoryShield.oledPaint();
    delay(1);
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i=0; i<accessoryShield.height()/2; i+=3) {
    // alternate colors
    accessoryShield.fillRect(i, i, accessoryShield.width()-i*2, accessoryShield.height()-i*2, color%2);
    accessoryShield.oledPaint();
    delay(1);
    color++;
  }
}

void testdrawtriangle(void) {
  for (int16_t i=0; i<min(accessoryShield.width(),accessoryShield.height())/2; i+=5) {
    accessoryShield.drawTriangle(accessoryShield.width()/2, accessoryShield.height()/2-i,
                     accessoryShield.width()/2-i, accessoryShield.height()/2+i,
                     accessoryShield.width()/2+i, accessoryShield.height()/2+i, WHITE);
    accessoryShield.oledPaint();
    delay(1);
  }
}

void testfilltriangle(void) {
  uint8_t color = WHITE;
  for (int16_t i=min(accessoryShield.width(),accessoryShield.height())/2; i>0; i-=5) {
    accessoryShield.fillTriangle(accessoryShield.width()/2, accessoryShield.height()/2-i,
                     accessoryShield.width()/2-i, accessoryShield.height()/2+i,
                     accessoryShield.width()/2+i, accessoryShield.height()/2+i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    accessoryShield.oledPaint();
    delay(1);
  }
}

void testdrawroundrect(void) {
  for (int16_t i=0; i<accessoryShield.height()/2-2; i+=2) {
    accessoryShield.drawRoundRect(i, i, accessoryShield.width()-2*i, accessoryShield.height()-2*i, accessoryShield.height()/4, WHITE);
    accessoryShield.oledPaint();
    delay(1);
  }
}

void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (int16_t i=0; i<accessoryShield.height()/2-2; i+=2) {
    accessoryShield.fillRoundRect(i, i, accessoryShield.width()-2*i, accessoryShield.height()-2*i, accessoryShield.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    accessoryShield.oledPaint();
    delay(1);
  }
}
   
void testdrawrect(void) {
  for (int16_t i=0; i<accessoryShield.height()/2; i+=2) {
    accessoryShield.drawRect(i, i, accessoryShield.width()-2*i, accessoryShield.height()-2*i, WHITE);
    accessoryShield.oledPaint();
    delay(1);
  }
}

void testdrawline() {  
  for (int16_t i=0; i<accessoryShield.width(); i+=4) {
    accessoryShield.drawLine(0, 0, i, accessoryShield.height()-1, WHITE);
    accessoryShield.oledPaint();
    delay(1);
  }
  for (int16_t i=0; i<accessoryShield.height(); i+=4) {
    accessoryShield.drawLine(0, 0, accessoryShield.width()-1, i, WHITE);
    accessoryShield.oledPaint();
    delay(1);
  }
  delay(250);
  
  accessoryShield.clearOledDisplay();
  for (int16_t i=0; i<accessoryShield.width(); i+=4) {
    accessoryShield.drawLine(0, accessoryShield.height()-1, i, 0, WHITE);
    accessoryShield.oledPaint();
    delay(1);
  }
  for (int16_t i=accessoryShield.height()-1; i>=0; i-=4) {
    accessoryShield.drawLine(0, accessoryShield.height()-1, accessoryShield.width()-1, i, WHITE);
    accessoryShield.oledPaint();
    delay(1);
  }
  delay(250);
  
  accessoryShield.clearOledDisplay();
  for (int16_t i=accessoryShield.width()-1; i>=0; i-=4) {
    accessoryShield.drawLine(accessoryShield.width()-1, accessoryShield.height()-1, i, 0, WHITE);
    accessoryShield.oledPaint();
    delay(1);
  }
  for (int16_t i=accessoryShield.height()-1; i>=0; i-=4) {
    accessoryShield.drawLine(accessoryShield.width()-1, accessoryShield.height()-1, 0, i, WHITE);
    accessoryShield.oledPaint();
    delay(1);
  }
  delay(250);

  accessoryShield.clearOledDisplay();
  for (int16_t i=0; i<accessoryShield.height(); i+=4) {
    accessoryShield.drawLine(accessoryShield.width()-1, 0, 0, i, WHITE);
    accessoryShield.oledPaint();
    delay(1);
  }
  for (int16_t i=0; i<accessoryShield.width(); i+=4) {
    accessoryShield.drawLine(accessoryShield.width()-1, 0, i, accessoryShield.height()-1, WHITE); 
    accessoryShield.oledPaint();
    delay(1);
  }
  delay(250);
}

void testscrolltext(void) {
  accessoryShield.setTextSize(2);
  accessoryShield.setTextColor(WHITE);
  accessoryShield.setCursor(10,0);
  accessoryShield.clearOledDisplay();
  accessoryShield.println("scroll");
  accessoryShield.oledPaint();
  delay(1);
 
  accessoryShield.startScrollRightOled(0x00, 0x0F);
  delay(2000);
  accessoryShield.stopScrollOled();
  delay(1000);
  accessoryShield.startScrollLeftOled(0x00, 0x0F);
  delay(2000);
  accessoryShield.stopScrollOled();
  delay(1000);    
  accessoryShield.startScrollDiagRightOled(0x00, 0x07);
  delay(2000);
  accessoryShield.startScrollDiagLeftOled(0x00, 0x07);
  delay(2000);
  accessoryShield.stopScrollOled();
}
