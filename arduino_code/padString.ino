#include <string_asukiaaa.h>

void setup() { Serial.begin(9600); }

void loop() {
  unsigned long nowMs = millis();
  Serial.println(string_asukiaaa::padStart(String(nowMs), 10, ' ') + "ms");
  Serial.println(string_asukiaaa::padEnd(String(nowMs), 10, ' ') + "ms");
  bool odd = nowMs % 2 == 1;
  Serial.println("odd? " + string_asukiaaa::yesNo(odd) + " " +
                 string_asukiaaa::trueFalse(odd));
  String strToSplit = "asukiaaa.blogspot.com";
  Serial.println(
      "0:" + string_asukiaaa::getBySplit(strToSplit, ".", 0));  // "asukiaaa"
  Serial.println(
      "1:" + string_asukiaaa::getBySplit(strToSplit, ".", 1));  // "blogspot"
  Serial.println("2:" +
                 string_asukiaaa::getBySplit(strToSplit, ".", 2));  // "com"
  Serial.println("3:" + string_asukiaaa::getBySplit(strToSplit, ".", 3));  // ""
  delay(500);
}
