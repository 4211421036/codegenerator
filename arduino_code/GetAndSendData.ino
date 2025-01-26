#include <FireBase32.h>

FireBase32 db("DB_URL", "DB_API_KEY");
int dataDB;

void setup() {
  Serial.begin(115200);
  db.wifi("SSID", "PASSWORD");
  db.WriteData(DATA_TO_SEND, "PATH_IN_DB");
  

}

void loop() {
  db.GetData("PATH_IN_DB", &dataDB);
  Serial.println();
  Serial.print(dataDB);
  delay(50);
}