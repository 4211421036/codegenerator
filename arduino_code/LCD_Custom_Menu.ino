/*
 * @brief This sketch demonstrates how to build a very simple and basic custom menu.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Braccio++.h>

/**************************************************************************************
 * DEFINE
 **************************************************************************************/

#define MARGIN_LEFT   0
#define MARGIN_TOP    0

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static const char * btnm_map[] = {"Option 1", "\n",
                                  "Option 2", "\n",
                                  "Option 3", "\n", "\0"
                                 };

/**************************************************************************************
 * FUNCTIONS
 **************************************************************************************/

void customMenu()
{
  Braccio.lvgl_lock();
  lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act());
  lv_btnmatrix_set_map(btnm1, btnm_map);
  lv_obj_align(btnm1, LV_ALIGN_CENTER, MARGIN_LEFT, MARGIN_TOP);
  Braccio.lvgl_unlock();
}

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(115200);
  for (auto const start = millis(); !Serial && ((millis() - start) < 5000); delay(10)) { }

  if (!Braccio.begin(customMenu)) {
    if (Serial) Serial.println("Braccio.begin() failed.");
    for(;;) { }
  }
}

void loop()
{
  
}
