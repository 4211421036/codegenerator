/*
 * Read and write some registers from the PF1550 PMIC.
 */

#include <Arduino_PF1550.h>

//#define Serial Serial1

void setup()
{
  Serial.begin(115200);
  while(!Serial);

  PMIC.begin();
  PMIC.debug(Serial);
}


void loop()
{
  PMIC.writePMICreg(Register::CHARGER_CHG_EOC_CNFG, 0x41);
  uint8_t reg_val = PMIC.readPMICreg(Register::CHARGER_CHG_EOC_CNFG);

  Serial.print("CHARGER_CHG_SNS register value: ");
  Serial.println(reg_val,HEX);
  
  reg_val = PMIC.readPMICreg(Register::CHARGER_VBUS_SNS);

  Serial.print("CHARGER_VBUS_SNS register value: ");
  Serial.println(reg_val,HEX);
  
  reg_val = PMIC.readPMICreg(Register::CHARGER_CHG_INT_OK);

  Serial.print("CHARGER_CHG_INT_OK register value: ");
  Serial.println(reg_val,HEX);
  Serial.println();
  Serial.println();

  delay(5000);
}
