#include <Accessories.h>

const int buttonPin = 2;     // broche du poussoir

AccessoryLight light;  // La DEL
PortOnePin port;  // La connexion entre l'Arduino et la DEL.

void setup() 
{
  Accessories::begin();

  port.begin(LED_BUILTIN, DIGITAL);  // broche 13 sur un Uno, broche digitale.

  light.begin(&port, 100); // On branche la DEL sur le port, et on lui assigne le No 100

  // initialise le poussoir comme entrée
  pinMode(buttonPin, INPUT);
}

void loop() 
{
  // Selon l'état du bouton, on allume ou éteint la DEL
  if (digitalRead(buttonPin)== HIGH) {
    light.LightOn();  // Allume la DEL
  } else {
    light.LightOff();  // Eteint la DEL
  }
}