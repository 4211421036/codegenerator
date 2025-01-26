#include <Braccio++.h>

// Joystick
#define JOYSTICK_LEFT        1
#define JOYSTICK_RIGHT       2
#define JOYSTICK_SEL_BUTTON  3
#define JOYSTICK_UP          4
#define JOYSTICK_DOWN        5
#define BUTTON_ENTER         6

// Colors
#define COLOR_TEAL       0x00878F
#define COLOR_LIGHT_TEAL 0x62AEB2
#define COLOR_ORANGE     0xE47128
#define COLOR_YELLOW     0xE5AD24
#define COLOR_WHITE      0xFFFFFF

// Variables
int motorID = 0; // Selected motor ID
float initialAngle = 0.0;
float currentAngle = initialAngle; // Selected motor current angle
float angles[6]; // All motors current angles

static const char * btnm_map[] = {"Motor 1", "Motor 2", "\n",
                                  "Motor 3", "Motor 4", "\n",
                                  "Motor 5", "Motor 6", "\0"
                                 };

lv_obj_t * btnm; // Motors button matrix
lv_obj_t * meter; // Gauge with selected motor angle
lv_meter_indicator_t * indic; // Indication of selected motor angle


// Event Handlers
static void eventHandlerMeter(lv_event_t * e) {
  uint32_t pressed_key = Braccio.getKey();

  if (pressed_key == BUTTON_ENTER) {
    motorID = 0; // No motor selected
    motorMenu(); // Load motor menu screen
    lv_obj_del(meter); // Delete the object
  }
  else {
    lv_meter_set_indicator_end_value(meter, indic, (int32_t)angles[motorID - 1]);
  }
}

static void eventHandlerMenu(lv_event_t * e) {
  lv_obj_t * obj = lv_event_get_target(e);
  uint32_t id = lv_btnmatrix_get_selected_btn(obj);

  motorID = id + 1;
  currentAngle = angles[motorID - 1];
  meterScreen();
  lv_obj_del(btnm);
}

// Screens functions
void meterScreen(void)
{
  Braccio.lvgl_lock();
  meter = lv_meter_create(lv_scr_act());

  lv_obj_center(meter);
  lv_obj_set_size(meter, 200, 200);

  /*Remove the circle from the middle*/
  lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);

  static lv_style_t style_meter;
  lv_style_init(&style_meter);
  lv_style_set_text_color(&style_meter, lv_color_hex(COLOR_TEAL));
  lv_obj_add_style(meter, &style_meter, 0);

  /*Add a scale first*/
  lv_meter_scale_t * scale = lv_meter_add_scale(meter);
  lv_meter_set_scale_ticks(meter, scale, 22, 2, 10, lv_color_hex(COLOR_ORANGE));
  lv_meter_set_scale_major_ticks(meter, scale, 1, 2, 20, lv_color_hex(COLOR_ORANGE), 20);
  lv_meter_set_scale_range(meter, scale, 0, 315, 315, 90);

  /*Add a arc indicator*/
  indic = lv_meter_add_arc(meter, scale, 10, lv_color_hex(COLOR_LIGHT_TEAL), 0);

  lv_obj_add_event_cb(meter, eventHandlerMeter, LV_EVENT_KEY, NULL);

  lv_meter_set_indicator_end_value(meter, indic, (int32_t)angles[motorID - 1]);
  Braccio.lvgl_unlock();

  Braccio.connectJoystickTo(meter);
}

void motorMenu() {
  Braccio.lvgl_lock();
  static lv_style_t style_bg;
  lv_style_init(&style_bg);
  lv_style_set_bg_color(&style_bg, lv_color_hex(COLOR_WHITE));

  static lv_style_t style_btn;
  lv_style_init(&style_btn);
  lv_style_set_bg_color(&style_btn, lv_color_hex(COLOR_YELLOW));
  lv_style_set_border_color(&style_btn, lv_color_hex(COLOR_LIGHT_TEAL));
  lv_style_set_border_width(&style_btn, 2);
  lv_style_set_text_color(&style_btn, lv_color_hex(COLOR_TEAL));

  btnm = lv_btnmatrix_create(lv_scr_act());
  lv_btnmatrix_set_map(btnm, btnm_map);
  lv_obj_align(btnm, LV_ALIGN_CENTER, 0, 0);

  lv_obj_add_style(btnm, &style_bg, 0);
  lv_obj_add_style(btnm, &style_btn, LV_PART_ITEMS);

  lv_btnmatrix_set_btn_ctrl(btnm, 0, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm, 1, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm, 2, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm, 3, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm, 4, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm, 5, LV_BTNMATRIX_CTRL_CHECKABLE);

  lv_btnmatrix_set_one_checked(btnm, true);

  lv_obj_add_event_cb(btnm, eventHandlerMenu, LV_EVENT_PRESSED, NULL);
  Braccio.lvgl_unlock();

  Braccio.connectJoystickTo(btnm);
}

void setup() {
  Braccio.begin(motorMenu);

  delay(500); // Waits for the Braccio initialization

  // Sets the initial angle for the motors
  for (int i = 1; i <= 6; i++) {
    Braccio.move(i).to(initialAngle);
    delay(1000); // Necessary to set the motor ID correctly
  }
}

void loop() {
  int pressedKey = Braccio.getKey();

  Braccio.positions(angles);
  currentAngle = angles[motorID - 1];

  // Check if the Joystick Left is pressed
  if (pressedKey == JOYSTICK_LEFT) {
    if (currentAngle < 315 ) {
      currentAngle += 10.00;
    } else {
      currentAngle = 315;
    }
  }

  // Check if the Joystick Right is pressed
  if (pressedKey == JOYSTICK_RIGHT) {
    if (currentAngle > 15 ) {
      currentAngle -= 10.00;
    } else {
      currentAngle = 0;
    }
  }

  Braccio.move(motorID).to(currentAngle); // Moves the motor to the new angle
  delay(100);
}
