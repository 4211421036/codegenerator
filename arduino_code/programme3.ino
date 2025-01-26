#include <Accessories.h>
#include <Commanders.h>

const int buttonPin = 2;     // broche du poussoir

ButtonsCommanderPush push; // Le bouton
AccessoryLight light; // La DEL
PortOnePin port;    // La connexion entre l'Arduino et la DEL.

void setup() 
{
  Commanders::begin();
  Accessories::begin();

  port.begin(LED_BUILTIN, DIGITAL);  // broche 13 sur un Uno, broche digitale.

  light.begin(&port, 100); // On branche la DEL sur le port, et on lui assigne le No 100

  // initialise le poussoir comme entrée
  // l'identifiant du bouton à 100 indique que c'est l'accessoire '100' qui sera affecté si le bouton est pressé.
  push.begin(100, buttonPin);
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