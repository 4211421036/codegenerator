#include <rs485_asukiaaa.h>

#define PIN_RS485_DE 5
#define PIN_RS485_RE 6
#define RS485_BAUDRATE 115200
#define RS485_DEVICE_ADDRESS 2
rs485_asukiaaa::ModbusRtu::Central modbus(&Serial1, PIN_RS485_DE, PIN_RS485_RE);

void setup() {
  modbus.begin(RS485_BAUDRATE);
}

void loop() {
  uint8_t data[] = { 0, 1, 2 };
  modbus.writeQuery(RS485_DEVICE_ADDRESS, rs485_asukiaaa::ModbusRtu::FnCode::Write, data, 3);
}
