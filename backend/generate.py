import sys
from pathlib import Path

def simulate_arduino_generation(prompt: str):
    cpp_code = f"""// sensor.cpp
#include "sensor.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// Inisialisasi fungsi
void tampilkanSuhu(float suhu) {{
  // logika tampilkan suhu di OLED
}}"""
    h_code = """// sensor.h
#ifndef SENSOR_H
#define SENSOR_H

void tampilkanSuhu(float suhu);

#endif"""
    ino_code = f"""// main.ino
#include <Wire.h>
#include "sensor.h"

void setup() {{
  Serial.begin(9600);
}}

void loop() {{
  float suhu = 25.0; // Contoh suhu
  tampilkanSuhu(suhu);
  delay(1000);
}}"""
    return cpp_code, h_code, ino_code

if __name__ == '__main__':
    prompt_file = sys.argv[1]
    output_dir = Path(sys.argv[2])
    prompt = Path(prompt_file).read_text()

    cpp, h, ino = simulate_arduino_generation(prompt)

    (output_dir / "sensor.cpp").write_text(cpp)
    (output_dir / "sensor.h").write_text(h)
    (output_dir / "main.ino").write_text(ino)
