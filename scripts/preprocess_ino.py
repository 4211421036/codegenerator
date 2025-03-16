import os
import json
import numpy as np
from sklearn.feature_extraction.text import TfidfVectorizer

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

def main():
    # Create data directory if not exists
    os.makedirs('data', exist_ok=True)

    # Load and process .ino files
    descriptions, paths = [], []
    for root, _, files in os.walk('arduino_code'):
        for file in files:
            if file.endswith('.ino'):
                path = os.path.join(root, file)
                try:
                    encoding = detect_encoding(path)
                    with open(path, 'r', encoding=encoding, errors='replace') as f:
                        code = f.read()
                    desc = extract_description(code)
                    descriptions.append(desc)
                    paths.append(path)
                except Exception as e:
                    print(f"Error processing {path}: {str(e)}")
                    continue

    # Create TF-IDF model
    vectorizer = TfidfVectorizer(max_features=1000, min_df=2, max_df=0.95)
    tfidf_matrix = vectorizer.fit_transform(descriptions)

    # Prepare data for serialization
    data = {
        'vocab': {k: int(v) for k, v in vectorizer.vocabulary_.items()},
        'idf': vectorizer.idf_.tolist(),
        'paths': paths,
        'descriptions': descriptions
    }

    # Save processed data
    save_processed_data(data, 'data/processed_data.json')
    print(f"Successfully processed {len(paths)} files")

if __name__ == "__main__":
    main()
