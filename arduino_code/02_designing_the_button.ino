#include <Braccio++.h>

// Arduino Colors
#define COLOR_TEAL       0x00878F
#define COLOR_LIGHT_TEAL 0x62AEB2
#define COLOR_ORANGE     0xE47128
#define COLOR_YELLOW     0xE5AD24
#define COLOR_WHITE      0xFFFFFF


void customMenu() {
  Braccio.lvgl_lock();
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_bg_color(&style, lv_color_hex(COLOR_WHITE));
  lv_style_set_border_color(&style, lv_color_hex(COLOR_TEAL));
  lv_style_set_border_width(&style, 5);
  lv_style_set_text_color(&style, lv_color_hex(COLOR_ORANGE));

  lv_obj_t * btn1 = lv_btn_create(lv_scr_act()); 
  lv_obj_set_size(btn1, 120, 75);
  
  lv_obj_t * label1 = lv_label_create(btn1);
  lv_label_set_text(label1, "BTN 1");
  
  lv_obj_align(btn1, LV_ALIGN_CENTER, 0, 0);
  lv_obj_center(label1);

  lv_obj_add_style(btn1, &style, 0);
  Braccio.lvgl_unlock();
}

void setup() {
  // put your setup code here, to run once:
  Braccio.begin(customMenu);
}

void loop() {
  // put your main code here, to run repeatedly:

}
