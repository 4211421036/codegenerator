import os
import json
import numpy as np
from sklearn.feature_extraction.text import TfidfVectorizer

# Utility Functions
def detect_encoding(file_path):
    """Detect file encoding using chardet"""
    import chardet
    with open(file_path, 'rb') as f:
        raw_data = f.read()
        result = chardet.detect(raw_data)
        return result['encoding'] if result['confidence'] > 0.9 else 'utf-8'

def extract_description(code):
    """Extract description from code comments"""
    comments = []
    for line in code.split('\n'):
        if line.strip().startswith('//'):
            clean_line = line.split('//')[1].strip()
            clean_line = clean_line.encode('ascii', 'ignore').decode()
            if clean_line:
                comments.append(clean_line)
    return ' '.join(comments[:5]) if comments else 'no_description'

def numpy_serializer(obj):
    """Custom serializer for numpy data types"""
    if isinstance(obj, (np.int64, np.int32)):
        return int(obj)
    elif isinstance(obj, np.float64):
        return float(obj)
    elif isinstance(obj, np.ndarray):
        return obj.tolist()
    raise TypeError(f"Object of type {type(obj)} is not JSON serializable")

def save_processed_data(data, file_path):
    """Save processed data with custom numpy serializer"""
    with open(file_path, 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False, default=numpy_serializer)

# Main Processing Function
def process_ino_files():
    """Process all .ino files in the arduino_code directory"""
    descriptions, paths = [], []
    processed_count = 0
    error_count = 0
    
    for root, _, files in os.walk('arduino_code'):
        for file in files:
            if file.endswith('.ino'):
                path = os.path.join(root, file)
                try:
                    # Detect and read file with correct encoding
                    encoding = detect_encoding(path)
                    with open(path, 'r', encoding=encoding, errors='replace') as f:
                        code = f.read()
                    
                    # Extract description and store data
                    desc = extract_description(code)
                    descriptions.append(desc)
                    paths.append(path)
                    processed_count += 1
                    
                except Exception as e:
                    print(f"Error processing {path}: {str(e)}")
                    error_count += 1
                    continue
    
    print(f"\nProcessing Summary:")
    print(f"  - Total files processed: {processed_count}")
    print(f"  - Files with errors: {error_count}")
    
    return descriptions, paths

def create_tfidf_model(descriptions):
    """Create and return TF-IDF vectorizer"""
    vectorizer = TfidfVectorizer(
        max_features=1000,
        min_df=2,
        max_df=0.95,
        stop_words='english'
    )
    vectorizer.fit_transform(descriptions)
    return vectorizer

def main():
    # Ensure data directory exists
    os.makedirs('data', exist_ok=True)
    
    # Process .ino files
    print("Starting preprocessing...")
    descriptions, paths = process_ino_files()
    
    # Create TF-IDF model
    vectorizer = create_tfidf_model(descriptions)
    
    # Prepare data for serialization
    data = {
        'vocab': {k: int(v) for k, v in vectorizer.vocabulary_.items()},
        'idf': vectorizer.idf_.tolist(),
        'paths': paths,
        'descriptions': descriptions
    }
    
    # Save processed data
    save_processed_data(data, 'data/processed_data.json')
    print(f"\nSuccessfully processed {len(paths)} files")
    print("Data saved to data/processed_data.json")

if __name__ == "__main__":
    main()
