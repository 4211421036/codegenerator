#include <Braccio++.h>

class BraccioFactoryClass: public BraccioClass {
public:
  void setID(int id) {
    BraccioClass::setID(id);
  }
};

BraccioFactoryClass factory;

void setup() {
  Serial.begin(115200);
  factory.begin();
  while (!Serial);
}

void loop() {
  Serial.println("Write the ID you want to use for this motor:");
  String ret = "";
  do {
    ret = Serial.readStringUntil('\n');
  } while (ret == "");

  int selected = ret.toInt();

  Serial.println("Writing ID " + String(selected));
  factory.setID(selected);
}
