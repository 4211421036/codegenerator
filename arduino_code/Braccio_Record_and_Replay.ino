/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Braccio++.h>

#include "AppState.h"

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

RecordAndReplayApp app;

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  if (Braccio.begin(custom_main_menu))
  {
    app.enableButtons();
    /* Allow greater angular velocity than the default one. */
    Braccio.setAngularVelocity(45.0f);
  }
}

void loop()
{
  /* Only execute every 50 ms. */
  static auto prev = millis();
  auto const now = millis();

  if ((now - prev) > 50)
  {
    prev = now;
    app.update(EventSource::TimerTick);
  }
}
