
import sys
import torch
from pathlib import Path
from zipfile import ZipFile

prompt = sys.argv[1]

# Load model
model = torch.load("model/arduino_model.pt_best.pt", map_location="cpu")
model.eval()

# Dummy output (replace this with your actual model logic)
with torch.no_grad():
    result = model(prompt) if callable(model) else "// Model inference result here\n"

# Create output folder
Path("output").mkdir(exist_ok=True)

# Create files with comments
with open("output/main.ino", "w") as f:
    f.write("// main.ino - fungsi utama Arduino\n")
    f.write(result)

with open("output/module.h", "w") as f:
    f.write("// module.h - deklarasi fungsi\n")

with open("output/module.cpp", "w") as f:
    f.write("// module.cpp - implementasi fungsi\n")

# Zip the output files
with ZipFile("output/kode.zip", "w") as zipf:
    zipf.write("output/main.ino", arcname="main.ino")
    zipf.write("output/module.h", arcname="module.h")
    zipf.write("output/module.cpp", arcname="module.cpp")
