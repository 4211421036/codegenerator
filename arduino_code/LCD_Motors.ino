/*
 * @brief This sketch demonstrates how to build a very simple
 * and basic custom menu interacting with the Braccio++ carriers
 * joystick, allowing to control a specific motor by selecting
 * it from the menu.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Braccio++.h>

/**************************************************************************************
 * VARIABLES
 **************************************************************************************/

int selected_motor = 0;

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static const char * btnm_map[] = {"Motor 1", "Motor 2", "\n",
                                  "Motor 3", "Motor 4", "\n",
                                  "Motor 5", "Motor 6", "\n", "\0"
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
    Serial.println(txt);
    if (strcmp(txt, "Motor 1") == 0) {
      selected_motor = 1;
      Braccio.pingOff();
    } else if (strcmp(txt, "Motor 2") == 0) {
      selected_motor = 2;
      Braccio.pingOff();
    } else if (strcmp(txt, "Motor 3") == 0) {
      selected_motor = 3;
      Braccio.pingOff();
    } else if (strcmp(txt, "Motor 4") == 0) {
      selected_motor = 4;
      Braccio.pingOff();
    } else if (strcmp(txt, "Motor 5") == 0) {
      selected_motor = 5;
      Braccio.pingOff();
    } else if (strcmp(txt, "Motor 6") == 0) {
      selected_motor = 6;
      Braccio.pingOff();
    } else {
      Braccio.pingOn();
      selected_motor = 0;
    }
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
  lv_obj_align(btnm1, LV_ALIGN_CENTER, 0, 0);
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
  for (float angle = 0.0; angle <= 180.0; angle+=10.0){
    Braccio.move(selected_motor).to(angle);
    delay(500);
  }
}
