/*
 * Test low power capability of Portenta C33.
 */

#include <Arduino_PF1550.h>

#include <Wire.h>

#include "RTC.h"
#include "r_lpm.h"

void periodic_cbk()
{
 digitalWrite(LEDR, !digitalRead(LEDR));
}

void alarm_cbk()
{
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
}

lpm_instance_ctrl_t p_api_ctrl;
lpm_cfg_t p_cfg;

void setup()
{
  Serial.begin(9600);
  while (!Serial) { }

  //PMIC.debug(Serial);
  PMIC.begin();
  PMIC.configLDO1(Ldo1Voltage::V_3_30, false, false, false);
  PMIC.configLDO2(Ldo2Voltage::V_3_30, false, false, false);
  PMIC.configLDO3(Ldo3Voltage::V_1_20, false, false, false);
  PMIC.configSw2(Sw2Voltage::V_3_30, Sw2Voltage::V_3_30, Sw2Voltage::V_3_30, Sw2CurrentLimit::I_1_5_A, false, false, false);

  Serial.print("Device ID = ");
  Serial.println(PMIC.getDeviceId(), HEX);

  p_cfg.low_power_mode = LPM_MODE_DEEP; //  LPM_MODE_SLEEP  LPM_MODE_STANDBY    LPM_MODE_STANDBY_SNOOZE    LPM_MODE_DEEP
  p_cfg.standby_wake_sources = LPM_STANDBY_WAKE_SOURCE_IRQ0 | LPM_STANDBY_WAKE_SOURCE_RTCALM;
  p_cfg.dtc_state_in_snooze = LPM_SNOOZE_DTC_DISABLE; // LPM_SNOOZE_DTC_ENABLE LPM_SNOOZE_DTC_DISABLE
  p_cfg.power_supply_state = LPM_POWER_SUPPLY_DEEPCUT0; // LPM_POWER_SUPPLY_DEEPCUT0, LPM_POWER_SUPPLY_DEEPCUT1, LPM_POWER_SUPPLY_DEEPCUT3
  p_cfg.output_port_enable = LPM_OUTPUT_PORT_ENABLE_RETAIN; // LPM_OUTPUT_PORT_ENABLE_RETAIN LPM_OUTPUT_PORT_ENABLE_HIGH_IMPEDANCE
  p_cfg.io_port_state = LPM_IO_PORT_NO_CHANGE;  // LPM_IO_PORT_NO_CHANGE LPM_IO_PORT_RESET
  p_cfg.deep_standby_cancel_source = LPM_DEEP_STANDBY_CANCEL_SOURCE_RTC_ALARM;

  R_LPM_Open(&p_api_ctrl, &p_cfg);

  /* Configure LED_BUILTIN. */
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  /* Configure the red RGB LED. */
  pinMode(LEDR, OUTPUT);
  digitalWrite(LEDR, LOW);
  delay(200);
  digitalWrite(LEDR, HIGH);

  /* Initialize the RTC. */
  RTC.begin();
  RTCTime initial_time(1, Month::JANUARY, 2000, 12, 10, 00, DayOfWeek::TUESDAY, SaveLight::SAVING_TIME_ACTIVE);

  if (!RTC.isRunning())
    RTC.setTime(initial_time);

  RTCTime alarm_time;
  alarm_time.setSecond(35);

  AlarmMatch alarm_match;
  alarm_match.addMatchSecond();

  if (!RTC.setPeriodicCallback(periodic_cbk, Period::ONCE_EVERY_2_SEC))
    Serial.println("ERROR: periodic callback not set");

  if (!RTC.setAlarmCallback(alarm_cbk, alarm_time, alarm_match))
    Serial.println("ERROR: alarm callback not set");
}

void loop()
{
  /* Enter low power mode. Note: The JLink looses connection here. */
  R_LPM_LowPowerModeEnter(&p_api_ctrl);

  if (RTC.isRunning())
  {
    /* GET CURRENT TIME FROM RTC */
    RTCTime current_time;
    RTC.getTime(current_time);

    /* PRINT CURRENT TIME on Serial */
    Serial.print("Current time: ");
    /* DATE */
    Serial.print(current_time.getDayOfMonth());
    Serial.print("/");
    Serial.print(Month2int(current_time.getMonth()));
    Serial.print("/");
    Serial.print(current_time.getYear());
    Serial.print(" - ");
    Serial.print(current_time.getUnixTime());
    Serial.print(" - ");

    struct timeval tv;
    gettimeofday(&tv, NULL);
    Serial.print(tv.tv_sec);
    Serial.print(" - ");

    /* HOUR:MINUTES:SECONDS */
    Serial.print(current_time.getHour());
    Serial.print(":");
    Serial.print(current_time.getMinutes());
    Serial.print(":");
    Serial.println(current_time.getSeconds());

    delay(1000);
  }
}
