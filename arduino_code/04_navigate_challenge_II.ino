#include <Braccio++.h>

#define MARGIN_LEFT   0
#define MARGIN_TOP    0

// Arduino Colors
#define COLOR_TEAL       0x00878F
#define COLOR_LIGHT_TEAL 0x62AEB2
#define COLOR_ORANGE     0xE47128
#define COLOR_YELLOW     0xE5AD24
#define COLOR_WHITE      0xFFFFFF

static const char * btnm_map[] = {"Option 1", "Option 2", "\n",
                                  "Option 3", "Option 4", "\n",
                                  "Option 5", "Option 6", "\0"
                                 };

static void eventHandler(lv_event_t * e) {
  Braccio.lvgl_lock();
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  if (code == LV_EVENT_RELEASED) {
    uint32_t id = lv_btnmatrix_get_selected_btn(obj);
    const char * txt = lv_btnmatrix_get_btn_text(obj, id);

    LV_LOG_USER("%s was released\n", txt);
    Serial.println(String(txt) + " was released.");
  }
  Braccio.lvgl_unlock();
}

void customMenu() {
  Braccio.lvgl_lock();
  static lv_style_t style_bg;
  lv_style_init(&style_bg);
  lv_style_set_bg_color(&style_bg, lv_color_hex(COLOR_LIGHT_TEAL));

  static lv_style_t style_btn;
  lv_style_init(&style_btn);
  lv_style_set_bg_color(&style_btn, lv_color_hex(COLOR_WHITE));
  lv_style_set_border_color(&style_btn, lv_color_hex(COLOR_YELLOW));
  lv_style_set_border_width(&style_btn, 2);
  lv_style_set_text_color(&style_btn, lv_color_hex(COLOR_TEAL));


  lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act());
  lv_btnmatrix_set_map(btnm1, btnm_map);
  lv_obj_align(btnm1, LV_ALIGN_CENTER, MARGIN_LEFT, MARGIN_TOP);

  lv_obj_add_style(btnm1, &style_bg, 0);
  lv_obj_add_style(btnm1, &style_btn, LV_PART_ITEMS);

  lv_btnmatrix_set_btn_ctrl(btnm1, 0, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm1, 1, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm1, 2, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm1, 3, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm1, 4, LV_BTNMATRIX_CTRL_CHECKABLE);
  lv_btnmatrix_set_btn_ctrl(btnm1, 5, LV_BTNMATRIX_CTRL_CHECKABLE);

  lv_btnmatrix_set_one_checked(btnm1, true);

  lv_obj_add_event_cb(btnm1, eventHandler, LV_EVENT_ALL, NULL);
  Braccio.lvgl_unlock();

  Braccio.connectJoystickTo(btnm1);
}

void setup() {
  Braccio.begin(customMenu);
}

void loop() {

}
