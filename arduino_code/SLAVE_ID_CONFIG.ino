/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with M5Atom sample source code
*                          配套 M5Atom 示例源代码
*
* Visit for more information: https://docs.m5stack.com/en/unit/acssr
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/unit/acssr
*
* Product: ACSSR
* Date: 2022/7/5
*******************************************************************************
  Modbus ID verification
  Modbus ID 确认
  Libraries:
    - [ArduinoModbus](https://github.com/m5stack/ArduinoModbus)
    - [Arduino485](https://github.com/m5stack/ArduinoRS485)
*/

#include "M5Atom.h"
#include "M5_ACSSR.h"
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

#define NEW_SLAVE_ID 0x0050

// RS485Class RS485(Serial2, 26, 32, -1, -1);
RS485Class RS485(Serial2, 22, 19, -1, -1);

uint8_t slave_id = 0;

bool scan_addr() {
    // scan addr
    Serial.printf("Scan Slave ID: \r\n");
    for (uint8_t id = 1; id < 247; id++) {
        Serial.printf(".");
        if (ModbusRTUClient.requestFrom(id, HOLDING_REGISTERS,
                                        ACSSR_VERSION_HOLDING_ADDR, 1)) {
            Serial.print("\r\nfirmware version: ");
            while (ModbusRTUClient.available()) {
                Serial.printf("%02X", ModbusRTUClient.read());
            }
            Serial.printf("\r\n");
            Serial.printf("Slave ID is: 0x%02X\r\n", id);
            slave_id = id;
            return true;
            break;
        }
    }
    Serial.println("Not Found Slave ID");
    return false;
}

void setup() {
    M5.begin(true, false, true);
    delay(2000);
    M5.dis.fillpix(0x0000ff);
    ModbusRTUClient.begin(115200, SERIAL_8N1);

    ModbusRTUClient.setTimeout(50);
    while (!scan_addr())
        ;
    Serial.println("Click Btn to Change Slave ID");
    while (1) {
        if (M5.Btn.wasPressed()) {
            if (ModbusRTUClient.holdingRegisterWrite(
                    slave_id, ACSSR_ID_HOLDING_ADDR, NEW_SLAVE_ID)) {
                Serial.printf("Change Slave ID to: 0x%02X\r\n", NEW_SLAVE_ID);
                slave_id = NEW_SLAVE_ID;
                break;
            } else {
                Serial.println("Change Slave ID: FAIL");
            };
        };
        M5.update();
    }
}

void loop() {
    scan_addr();
    delay(1000);
}
