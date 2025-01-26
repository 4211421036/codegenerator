/*
 * @brief Activating the "MOVE" button by pressing
 * the joystick enables a waving motion of the arm.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Braccio++.h>

/**************************************************************************************
 * GLOBAL CONSTANTS
 **************************************************************************************/

float const home_position[6] = {SmartServoClass::MAX_ANGLE / 2.0f,
                                SmartServoClass::MAX_ANGLE / 2.0f,
                                SmartServoClass::MAX_ANGLE / 2.0f,
                                SmartServoClass::MAX_ANGLE / 2.0f,
                                SmartServoClass::MAX_ANGLE / 2.0f,
                                90.0f};
static const char * btnm_map[] = {"Move", "\0"};

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

bool move_joint = false;

/**************************************************************************************
 * FUNCTIONS
 **************************************************************************************/

static void event_handler(lv_event_t * e)
{
  Braccio.lvgl_lock();
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED)
  {
    uint32_t id = lv_btnmatrix_get_selected_btn(obj);
    const char * txt = lv_btnmatrix_get_btn_text(obj, id);

    LV_LOG_USER("%s was pressed\n", txt);
    if (Serial) Serial.println(txt);

    if (strcmp(txt, "Move") == 0)
      move_joint = !move_joint;
  }
  Braccio.lvgl_unlock();
}

void customMenu()
{
  Braccio.lvgl_lock();
  lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act());
  lv_btnmatrix_set_map(btnm1, btnm_map);
  lv_btnmatrix_set_btn_ctrl(btnm1, 0, LV_BTNMATRIX_CTRL_CHECKABLE);
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

  Braccio.moveTo(home_position[0], home_position[1], home_position[2], home_position[3], home_position[4], home_position[5]);
  delay(1000);
  Braccio.setAngularVelocity(45.0f); /* 45 deg/sec */
}

void loop()
{
  if (move_joint)
  {
    Braccio.move(4).to((SmartServoClass::MAX_ANGLE / 2.0f) - 45.0f);
    delay(2000);
    Braccio.move(4).to((SmartServoClass::MAX_ANGLE / 2.0f) + 45.0f);
    delay(2000);
  }
}
