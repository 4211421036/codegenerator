import os
import chardet
from collections import defaultdict

def check_encodings():
    encoding_stats = defaultdict(int)
    problematic_files = []
    
    for root, _, files in os.walk('arduino_code'):
        for file in files:
            if file.endswith('.ino'):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'rb') as f:
                        rawdata = f.read()
                        result = chardet.detect(rawdata)
                        encoding_stats[result['encoding']] += 1
                        
                        # Log problematic files
                        if result['confidence'] < 0.9:
                            problematic_files.append({
                                'path': file_path,
                                'encoding': result['encoding'],
                                'confidence': result['confidence']
                            })
                except Exception as e:
                    print(f"Error processing {file_path}: {str(e)}")
                    continue
    
    # Print summary
    print("\nEncoding Statistics:")
    for encoding, count in encoding_stats.items():
        print(f"{encoding}: {count} files")
    
    if problematic_files:
        print("\nProblematic Files:")
        for file in problematic_files:
            print(f"{file['path']} - Encoding: {file['encoding']} (Confidence: {file['confidence']:.2f})")
    
    return encoding_stats

if __name__ == "__main__":
    check_encodings()
