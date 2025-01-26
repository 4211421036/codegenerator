#include <Accessories.h>
#include <Commanders.h>
 
#define TEMPO  20000

// L'ILS ...
ButtonsCommanderPush boutonILS;
 
// Le moteur
AccessoryServo PN;

// Les DELS
AccessoryLightMulti Dels;
 
// Les ports pour connecter le moteur et les DELs.
PortServo portPN;
PortOnePin portLight1;
PortOnePin portLight2;
 
void setup()
{
  Commanders::begin(LED_BUILTIN);

  // Memoriser les positions des moteurs dans l'EEPROM.
  Accessories::begin(0, 500);
 
  // Un seul bouton, mais deux événements pour le PN
  boutonILS.begin(1234, 6); // premier événement pour le déclanchement
 
  // Les ports avec leurs broches en digital (pas PWM)
  portPN.begin(14);
  portLight1.begin(7, DIGITAL_INVERTED);
  portLight2.begin(8, DIGITAL_INVERTED);

  // begin de l'accessoire de 2 Dels qui doivent clignoter à 500ms d'intervalle. 
  Dels.begin(0, 2, 500);

  // Les DELs avec pour chaque, le port utilisé. 
  Dels.beginLight(0, &portLight1);
  Dels.beginLight(1, &portLight2);

  // On fait clignoter les leds...
  Dels.SetBlinking(0, 500);
  Dels.SetBlinking(1, 500);

  // Le servo : pas de durée de mouvement, un débattement entre 95 et 135 degres
  // et deux positions stables annoncées avec des identifiants inutiles (mais obligatoires)
  PN.begin(&portPN, 50, 95, 135, 1);
  // Les deux positions sont au mini et au maxi :
  PN.AddMinMaxMovingPositions(456, 789);
}
 
unsigned long dateFermeture = 0;

void loop()
{
  Accessories::loop();

  unsigned long id = Commanders::loop();
 
  // si l'ils est activé !
  if (id == 1234)
  {
    if (dateFermeture== 0)
    {
      dateFermeture = millis();

      // mise en route clignotement
      Dels.Blink();

      // mouvement servo : fermeture
      PN.MoveMinimum();

      // Plus rien à faire, attendons l'étape suivante.
      return;
    }
    else
    {
      dateFermeture = millis();
    }
  }

  if (dateFermeture > 0)
  {
	  if (millis() - dateFermeture > TEMPO)
	  {
		  // Fin de l'animation. On ouvre les barrières et on éteint les Dels...
		  dateFermeture = 0;
		  PN.MoveMaximum();
		  Dels.LightOff();
	  }
  }
}