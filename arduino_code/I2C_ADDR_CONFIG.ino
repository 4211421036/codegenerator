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
  I2C address verification
  I2C 地址验证
*/
#include "M5Atom.h"
#include "M5_ACSSR.h"

#define NEW_I2C_ADDR 0x51

uint8_t i2c_addr = ACSSR_DEFAULT_ADDR;

M5_ACSSR SSR;

void scan_addr() {
    // scan addr
    Serial.printf("Scan I2C Addr: \r\n");
    for (uint8_t addr = 1; addr < 127; addr++) {
        Serial.printf(".");
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.printf("\r\n");
            Serial.printf("Device I2C Addr is: 0x%02X\r\n", addr);
            i2c_addr = addr;
            break;
        }
    }
}

void setup() {
    M5.begin(true, false, true);
    M5.dis.fillpix(0xff0000);
    Wire.begin(26, 32);

    while (!SSR.begin(&Wire, 26, 32, i2c_addr)) {
        Serial.println("ACSSR I2C INIT ERROR");
        scan_addr();
    }

    M5.dis.fillpix(0x0000ff);
    Serial.println("Click Btn to Change I2C Addr");
    while (1) {
        if (M5.Btn.wasPressed()) {
            if (SSR.setDeviceAddr(NEW_I2C_ADDR)) {
                Serial.printf("Change I2C Addr to: 0x%02X\r\n", NEW_I2C_ADDR);
                i2c_addr = NEW_I2C_ADDR;
                SSR.begin(&Wire, 26, 32, i2c_addr);
                break;
            } else {
                Serial.println("Change I2C Addr: FAIL");
            };
        };
        M5.update();
    }
}

void loop() {
    scan_addr();
    delay(1000);
}
