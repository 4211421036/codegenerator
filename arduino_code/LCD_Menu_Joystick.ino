/*
 * @brief This sketch demonstrates how to build a very simple
 * and basic custom menu interacting with the Braccio++ carriers
 * joystick.
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
 * CONSTANT
 **************************************************************************************/

static const char * btnm_map[] = {"Option 1", "Option 2", "\n",
                                  "Option 3", "Option 4", "\n",
                                  "Option 5", "Option 6", "\n", "\0"
                                 };

/**************************************************************************************
 * FUNCTIONS
 **************************************************************************************/

static void event_handler(lv_event_t * e)
{
  Braccio.lvgl_lock();
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED) {
    uint32_t id = lv_btnmatrix_get_selected_btn(obj);
    const char * txt = lv_btnmatrix_get_btn_text(obj, id);

    LV_LOG_USER("%s was pressed\n", txt);
    Serial.println(String(txt) + " was pressed.");
  }
  Braccio.lvgl_unlock();
}

void customMenu()
{
  Braccio.lvgl_lock();
  lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act());
  lv_btnmatrix_set_map(btnm1, btnm_map);
  lv_btnmatrix_set_btn_ctrl(btnm1, 0, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm1, 1, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm1, 2, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm1, 3, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm1, 4, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm1, 5, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_obj_align(btnm1, LV_ALIGN_CENTER, MARGIN_LEFT, MARGIN_TOP);
  lv_obj_add_event_cb(btnm1, event_handler, LV_EVENT_ALL, NULL);
  Braccio.lvgl_unlock();

  Braccio.connectJoystickTo(btnm1);
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
