#include <Braccio++.h>

#define MARGIN_LEFT   0
#define MARGIN_TOP    0

#define COLOR_BG         0xA69F80
#define COLOR_BTN        0xF2C36B
#define COLOR_BORDER     0x591202
#define COLOR_TEXT       0xA65221

static const char * btnm_map[] = {"BTN 1", "\n",
                                  "BTN 2", "BTN 3", "\n",
                                  "BTN 4", "BTN 5", "\0"
                                 };

void customMenu() {
  Braccio.lvgl_lock();
  static lv_style_t style_bg;
  lv_style_init(&style_bg);
  lv_style_set_bg_color(&style_bg, lv_color_hex(COLOR_BG));

  static lv_style_t style_btn;
  lv_style_init(&style_btn);
  lv_style_set_bg_color(&style_btn, lv_color_hex(COLOR_BTN));
  lv_style_set_border_color(&style_btn, lv_color_hex(COLOR_BORDER));
  lv_style_set_border_width(&style_btn, 2);
  lv_style_set_text_color(&style_btn, lv_color_hex(COLOR_TEXT));
  
  
  lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act());
  lv_btnmatrix_set_map(btnm1, btnm_map);
  lv_btnmatrix_set_btn_width(btnm1, 1, 2);  // Make "Button 2" twice as wide as "Button 3"
  lv_obj_align(btnm1, LV_ALIGN_CENTER, MARGIN_LEFT, MARGIN_TOP);
  
  lv_obj_add_style(btnm1, &style_bg, 0);
  lv_obj_add_style(btnm1, &style_btn, LV_PART_ITEMS);
  Braccio.lvgl_unlock();
}

void setup() {
  // put your setup code here, to run once:
  Braccio.begin(customMenu);
}

void loop() {
  // put your main code here, to run repeatedly:

}
