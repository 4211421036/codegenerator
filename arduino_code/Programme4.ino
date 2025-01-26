#include <Accessories.h>
#include <Commanders.h>

// Les boutons du TCO...
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

void setup()
{
  Commanders::begin(LED_BUILTIN);
  // Memoriser les positions des moteurs dans l'EEPROM.
  Accessories::begin(0, 500);

  // Activation de la reception de messages Dcc
  // Ne fera rien en analogique... Mais on peut carrement enlever la ligne si besoin de mémoire.
  // Le dernier argument à true, c'est pour voir la DEL LED_BUILTIN clignoter si un signal Dcc est présent.
  // Mieux vaut remettre à false une fois le programme mis au point.
  DccCommander.begin(0x00, 0x00, digitalPinToInterrupt(3), true);

  // Gestion des boutons pour le TCO.
  // Ne fera rien si aucun bouton n'est branché, mais on peut enlever si besoin de mémoire.

  // Le switch d'aiguillage a deux positions, chacune associée à un Id Dcc.
  boutonAiguillageGauche.begin();
  // dcc 15 /0 et broche 2
  boutonAiguillageGauche.AddEvent(DCCINT(15, 0), 2);
  // dcc 15 / 1 et broche 7
  boutonAiguillageGauche.AddEvent(DCCINT(15, 1), 7);

  boutonAiguillageDroite.begin();
  boutonAiguillageDroite.AddEvent(DCCINT(16, 0), 4);
  boutonAiguillageDroite.AddEvent(DCCINT(16, 1), 5);

  // Un seul evenement pour le découpleur
  boutonDecoupleur.begin(DCCINT(17, 0), 6);

  // Les ports avec leurs broches en digital (pas PWM)
  portAiguillageGauche.begin(8, 9, DIGITAL);
  portAiguillageDroite.begin(10, 11, DIGITAL);
  portDecoupleur.begin(12, DIGITAL);

  // Les accessoires avec pour chaque, le port utilisé et le ou les identifiants associés à chaque position. 
  // 255 est la vitesse, ici au maxi, et 400 est la durée d'activation du moteur en millisecondes.
  // C'est encore un peu long pour des solénoïdes. Il faudra adapter selon le modèle de moteur...
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