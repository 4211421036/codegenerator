#include <Accessories.h>
#include <Commanders.h>
 
// Les boutons du TCO...
ButtonsCommanderSwitch boutonCroisement;
 
// Les moteurs
AccessoryMotorTwoWays aiguillageHautGauche;
AccessoryMotorTwoWays aiguillageHautDroite;
AccessoryMotorTwoWays aiguillageBasGauche;
AccessoryMotorTwoWays aiguillageBasDroite;
 
// Le groupe pour les moteurs
AccessoryGroup groupeCroisement;
 
// Les ports pour connecter les moteurs.
PortTwoPins portAiguillageHautGauche;
PortTwoPins portAiguillageHautDroite;
PortTwoPins portAiguillageBasGauche;
PortTwoPins portAiguillageBasDroite;
 
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
 
  // Le switch de croisement a deux positions, chacune associée à un Id Dcc.
  boutonCroisement.begin();
  // dcc 15 /0 et broche 4 pour l'interrupteur
  boutonCroisement.AddEvent(DCCINT(15, 0), 4);
  // dcc 15 / 1 et broche 5 pour l'interrupteur
  boutonCroisement.AddEvent(DCCINT(15, 1), 5);
 
  // Les ports avec leurs broches en digital (pas PWM)
  portAiguillageHautGauche.begin(7, 8, DIGITAL);
  portAiguillageHautDroite.begin(9, 10, DIGITAL);
  portAiguillageBasGauche.begin(11, 12, DIGITAL);
  portAiguillageBasDroite.begin(14, 15, DIGITAL);
 
  // Les accessoires avec pour chaque, le port utilisé et le ou les identifiants associés à chaque position. 
  // 255 est la vitesse, ici au maxi, et 400 est la durée d'activation du moteur en millisecondes.
  // C'est encore un peu long pour des solénoïdes. Il faudra adapter selon le modèle de moteur...
  // Les identifiants sont volontairement non significatifs (1000 et plus...)
  // puisqu'aucun événement ne doit faire bouger ces moteurs individuellement.
  aiguillageHautGauche.beginTwoWays(&portAiguillageHautGauche, 1000, 1001, 255, 400);
  aiguillageHautDroite.beginTwoWays(&portAiguillageHautDroite, 1002, 1003, 255, 400);
  aiguillageBasGauche.beginTwoWays(&portAiguillageBasGauche, 1004, 1005, 255, 400);
  aiguillageBasDroite.beginTwoWays(&portAiguillageBasDroite, 1006, 1007, 255, 400);
 
  // Fabriquons le groupe de quatre moteurs à deux états
  groupeCroisement.begin();
 
  // Premier état, tous les moteurs droits.
  // L'argument 'false' signifie pas d'exécution simultanée. Les moteurs bougeront les uns après les autres.
  groupeCroisement.AddState(DCCINT(15, 0), false);
  // Arbitrairement, LEFT signifie droit (première broche activée)
  groupeCroisement.AddStateItem(DCCINT(15, 0), aiguillageHautGauche, LEFT);
  groupeCroisement.AddStateItem(DCCINT(15, 0), aiguillageHautDroite, LEFT);
  groupeCroisement.AddStateItem(DCCINT(15, 0), aiguillageBasGauche, LEFT);
  groupeCroisement.AddStateItem(DCCINT(15, 0), aiguillageBasDroite, LEFT);
 
  // Second état, tous les moteurs déviés.
  // Arbitrairement, RIGHT signifie dévié (seconde broche activée)
  groupeCroisement.AddState(DCCINT(15, 1), false);
  groupeCroisement.AddStateItem(DCCINT(15, 1), aiguillageHautGauche, RIGHT);
  groupeCroisement.AddStateItem(DCCINT(15, 1), aiguillageHautDroite, RIGHT);
  groupeCroisement.AddStateItem(DCCINT(15, 1), aiguillageBasGauche, RIGHT);
  groupeCroisement.AddStateItem(DCCINT(15, 1), aiguillageBasDroite, RIGHT);
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