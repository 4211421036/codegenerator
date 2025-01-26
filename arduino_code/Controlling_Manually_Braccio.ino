/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Braccio++.h>

#include "AppState.h"

/**************************************************************************************
 * DEFINES
 **************************************************************************************/

#define COLOR_TEAL       0x00878F
#define COLOR_LIGHT_TEAL 0x62AEB2
#define COLOR_YELLOW     0xE5AD24

/**************************************************************************************
 * TYPEDEF
 **************************************************************************************/

// IDs of the displayed directional UI buttons
enum BUTTONS {
  BTN_UP = 1,
  BTN_DOWN = 7,
  BTN_LEFT = 3,
  BTN_RIGHT = 5,
};

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static float const HOME_POS[6] = {157.5, 157.5, 157.5, 157.5, 157.5, 90.0};
static const char *DIRECTION_BTNM_MAP[] = {" ", LV_SYMBOL_UP, " ", "\n",
                                           LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, "\n",
                                           " ", LV_SYMBOL_DOWN, " ", "\0"};

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

lv_obj_t * direction_btnm;
lv_obj_t * label;

ManualControlApp app;

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(115200);

  if (Braccio.begin(directionScreen))
  {
    /* Move to home position. */
    Braccio.moveTo(HOME_POS[0], HOME_POS[1], HOME_POS[2], HOME_POS[3], HOME_POS[4], HOME_POS[5]);
    delay(500);
    /* Init state. */
    app.update(Button::None);
    /* Enable buttons. */
    Braccio.lvgl_lock();
    lv_btnmatrix_clear_btn_ctrl(direction_btnm, 1, LV_BTNMATRIX_CTRL_DISABLED);
    lv_btnmatrix_clear_btn_ctrl(direction_btnm, 3, LV_BTNMATRIX_CTRL_DISABLED);
    lv_btnmatrix_clear_btn_ctrl(direction_btnm, 5, LV_BTNMATRIX_CTRL_DISABLED);
    lv_btnmatrix_clear_btn_ctrl(direction_btnm, 7, LV_BTNMATRIX_CTRL_DISABLED);
    Braccio.lvgl_unlock();
  }
}

void loop()
{
  /* Execute every 10 ms. */
  {
    static auto prev = millis();
    auto const now = millis();
    if ((now - prev) > 10)
    {
      prev = now;
      handle_ButtonPressedReleased();
    }
  }

  /* Execute every 50 ms. */
  {
    static auto prev = millis();
    auto const now = millis();
    if ((now - prev) > 50)
    {
      prev = now;

      if (Braccio.isJoystickPressed_UP())
        app.update(Button::Up);
      if (Braccio.isJoystickPressed_DOWN())
        app.update(Button::Down);
      if (Braccio.isJoystickPressed_LEFT())
        app.update(Button::Left);
      if (Braccio.isJoystickPressed_RIGHT())
        app.update(Button::Right);
    }
  }
}

/**************************************************************************************
 * FUNCTIONS
 **************************************************************************************/

void directionScreen(void)
{
  Braccio.lvgl_lock();
  
  static lv_style_t style_bg;
  lv_style_init(&style_bg);
  lv_style_set_bg_color(&style_bg, lv_color_white());

  static lv_style_t style_btn;
  lv_style_init(&style_btn);
  lv_style_set_bg_color(&style_btn, lv_color_hex(COLOR_LIGHT_TEAL));
  lv_style_set_text_color(&style_btn, lv_color_white());

  direction_btnm = lv_btnmatrix_create(lv_scr_act());
  lv_obj_set_size(direction_btnm, 240, 240);
  lv_btnmatrix_set_map(direction_btnm, DIRECTION_BTNM_MAP);
  lv_obj_align(direction_btnm, LV_ALIGN_CENTER, 0, 0);

  lv_obj_add_style(direction_btnm, &style_bg, 0);
  lv_obj_add_style(direction_btnm, &style_btn, LV_PART_ITEMS);

  lv_btnmatrix_set_btn_ctrl(direction_btnm, 0, LV_BTNMATRIX_CTRL_HIDDEN);
  lv_btnmatrix_set_btn_ctrl(direction_btnm, 1, LV_BTNMATRIX_CTRL_DISABLED);
  lv_btnmatrix_set_btn_ctrl(direction_btnm, 2, LV_BTNMATRIX_CTRL_HIDDEN);
  lv_btnmatrix_set_btn_ctrl(direction_btnm, 3, LV_BTNMATRIX_CTRL_DISABLED);
  lv_btnmatrix_set_btn_ctrl(direction_btnm, 4, LV_BTNMATRIX_CTRL_HIDDEN);
  lv_btnmatrix_set_btn_ctrl(direction_btnm, 5, LV_BTNMATRIX_CTRL_DISABLED);
  lv_btnmatrix_set_btn_ctrl(direction_btnm, 6, LV_BTNMATRIX_CTRL_HIDDEN);
  lv_btnmatrix_set_btn_ctrl(direction_btnm, 7, LV_BTNMATRIX_CTRL_DISABLED);
  lv_btnmatrix_set_btn_ctrl(direction_btnm, 8, LV_BTNMATRIX_CTRL_HIDDEN);

  lv_btnmatrix_set_one_checked(direction_btnm, true);
  lv_btnmatrix_set_selected_btn(direction_btnm, 1);

  label = lv_label_create(lv_scr_act());
  lv_obj_set_width(label, 240);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  lv_label_set_text(label, "");
  
  Braccio.lvgl_unlock();
}

void handle_ButtonPressedReleased()
{
  /* ENTER */

  static bool prev_joystick_pressed_enter = false;
  bool const curr_joystick_pressed_enter = Braccio.isButtonPressed_ENTER();
  if (!prev_joystick_pressed_enter && curr_joystick_pressed_enter) {
    app.update(Button::Enter);
  }
  prev_joystick_pressed_enter = curr_joystick_pressed_enter;

  /* SELECT */

  static bool prev_joystick_pressed_select = false;
  bool const curr_joystick_pressed_select = Braccio.isJoystickPressed_SELECT();
  if (!prev_joystick_pressed_select && curr_joystick_pressed_select) {
    app.update(Button::Enter);
  }
  prev_joystick_pressed_select = curr_joystick_pressed_select;

  /* DOWN */

  static bool prev_joystick_pressed_down = false;
  bool const curr_joystick_pressed_down = Braccio.isJoystickPressed_DOWN();
  if (!prev_joystick_pressed_down && curr_joystick_pressed_down) {
    handle_OnButtonDownPressed();
  }
  if (prev_joystick_pressed_down && !curr_joystick_pressed_down) {
    handle_OnButtonDownReleased();
  }
  prev_joystick_pressed_down = curr_joystick_pressed_down;

  /* UP */

  static bool prev_joystick_pressed_up = false;
  bool const curr_joystick_pressed_up = Braccio.isJoystickPressed_UP();
  if (!prev_joystick_pressed_up && curr_joystick_pressed_up) {
    handle_OnButtonUpPressed();
  }
  if (prev_joystick_pressed_up && !curr_joystick_pressed_up) {
    handle_OnButtonUpReleased();
  }
  prev_joystick_pressed_up = curr_joystick_pressed_up;

  /* LEFT */

  static bool prev_joystick_pressed_left = false;
  bool const curr_joystick_pressed_left = Braccio.isJoystickPressed_LEFT();
  if (!prev_joystick_pressed_left && curr_joystick_pressed_left) {
    handle_OnButtonLeftPressed();
  }
  if (prev_joystick_pressed_left && !curr_joystick_pressed_left) {
    handle_OnButtonLeftReleased();
  }
  prev_joystick_pressed_left = curr_joystick_pressed_left;

  /* RIGHT */

  static bool prev_joystick_pressed_right = false;
  bool const curr_joystick_pressed_right = Braccio.isJoystickPressed_RIGHT();
  if (!prev_joystick_pressed_right && curr_joystick_pressed_right) {
    handle_OnButtonRightPressed();
  }
  if (prev_joystick_pressed_right && !curr_joystick_pressed_right) {
    handle_OnButtonRightReleased();
  }
  prev_joystick_pressed_right = curr_joystick_pressed_right;
}

void handle_OnButtonDownPressed()
{
  Braccio.lvgl_lock();
  lv_btnmatrix_set_btn_ctrl(direction_btnm, BTN_DOWN, LV_BTNMATRIX_CTRL_CHECKED);
  Braccio.lvgl_unlock();
}

void handle_OnButtonDownReleased()
{
  Braccio.lvgl_lock();
  lv_btnmatrix_clear_btn_ctrl(direction_btnm, BTN_DOWN, LV_BTNMATRIX_CTRL_CHECKED);
  Braccio.lvgl_unlock();
}

void handle_OnButtonUpPressed()
{
  Braccio.lvgl_lock();
  lv_btnmatrix_set_btn_ctrl(direction_btnm, BTN_UP, LV_BTNMATRIX_CTRL_CHECKED);
  Braccio.lvgl_unlock();
}

void handle_OnButtonUpReleased()
{
  Braccio.lvgl_lock();
  lv_btnmatrix_clear_btn_ctrl(direction_btnm, BTN_UP, LV_BTNMATRIX_CTRL_CHECKED);
  Braccio.lvgl_unlock();
}

void handle_OnButtonLeftPressed()
{
  Braccio.lvgl_lock();
  lv_btnmatrix_set_btn_ctrl(direction_btnm, BTN_LEFT, LV_BTNMATRIX_CTRL_CHECKED);
  Braccio.lvgl_unlock();
}

void handle_OnButtonLeftReleased()
{
  Braccio.lvgl_lock();
  lv_btnmatrix_clear_btn_ctrl(direction_btnm, BTN_LEFT, LV_BTNMATRIX_CTRL_CHECKED);
  Braccio.lvgl_unlock();
}

void handle_OnButtonRightPressed()
{
  Braccio.lvgl_lock();
  lv_btnmatrix_set_btn_ctrl(direction_btnm, BTN_RIGHT, LV_BTNMATRIX_CTRL_CHECKED);
  Braccio.lvgl_unlock();
}

void handle_OnButtonRightReleased()
{
  Braccio.lvgl_lock();
  lv_btnmatrix_clear_btn_ctrl(direction_btnm, BTN_RIGHT, LV_BTNMATRIX_CTRL_CHECKED);
  Braccio.lvgl_unlock();
}
