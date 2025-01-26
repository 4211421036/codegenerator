 #include <Braccio++.h>

void customMenu() {
  Braccio.lvgl_lock();
  lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
  lv_obj_set_size(btn1, 120, 75);
  lv_obj_t * label1 = lv_label_create(btn1);
  lv_label_set_text(label1, "BTN 1");
  lv_obj_align(btn1, LV_ALIGN_CENTER, 0, 0);
  lv_obj_center(label1);
  Braccio.lvgl_unlock();
}

void setup() {
  // put your setup code here, to run once:
  Braccio.begin(customMenu);
}

void loop() {
  // put your main code here, to run repeatedly:

}
