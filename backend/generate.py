import sys
from pathlib import Path

def simulate_arduino_generation(prompt: str):
    cpp_code = f"""// module.cpp
#include "module.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// Inisialisasi fungsi
void tampilkanSuhu(float suhu) {{
  // logika tampilkan suhu di OLED
}}"""

    h_code = """// module.h
#ifndef MODULE_H
#define MODULE_H

void tampilkanSuhu(float suhu);

#endif"""

    ino_code = f"""// main.ino
#include <Wire.h>
#include "module.h"

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
    if len(sys.argv) < 3:
        print("Usage: generate.py <prompt_file> <output_dir>")
        sys.exit(1)

    prompt_file = Path(sys.argv[1])
    output_dir = Path(sys.argv[2])

    if not prompt_file.exists():
        print("Prompt file not found.")
        sys.exit(1)

    prompt = prompt_file.read_text().strip()

    cpp_code, h_code, ino_code = simulate_arduino_generation(prompt)

    (output_dir / "module.cpp").write_text(cpp_code)
    (output_dir / "module.h").write_text(h_code)
    (output_dir / "main.ino").write_text(ino_code)

    # Optional: buat ZIP
    import zipfile
    with zipfile.ZipFile(output_dir / "kode.zip", 'w') as zipf:
        zipf.writestr("main.ino", ino_code)
        zipf.writestr("module.cpp", cpp_code)
        zipf.writestr("module.h", h_code)
