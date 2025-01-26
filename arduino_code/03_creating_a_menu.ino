#include <Braccio++.h>

#define MARGIN_LEFT   0
#define MARGIN_TOP    0

// Arduino Colors
#define COLOR_TEAL       0x00878F
#define COLOR_LIGHT_TEAL 0x62AEB2
#define COLOR_ORANGE     0xE47128
#define COLOR_YELLOW     0xE5AD24
#define COLOR_WHITE      0xFFFFFF

static const char * btnm_map[] = {"OPTION 1", "OPTION 2", "\n",
                                  "OPTION 3", "OPTION 4", "\0"
                                 };

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
  Braccio.lvgl_unlock();
}

void setup() {
  // put your setup code here, to run once:
  Braccio.begin(customMenu);
}

void loop() {
  // put your main code here, to run repeatedly:

}
