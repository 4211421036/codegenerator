import chardet

def check_encodings():
    encodings = []
    for root, _, files in os.walk('arduino_code'):
        for file in files:
            if file.endswith('.ino'):
                with open(os.path.join(root, file), 'rb') as f:
                    rawdata = f.read()
                    encodings.append(chardet.detect(rawdata)['encoding'])
    return set(encodings)

print("Detected encodings:", check_encodings())
