import os
import json
from sklearn.feature_extraction.text import TfidfVectorizer

# Create data directory if not exists
os.makedirs('data', exist_ok=True)

def detect_encoding(file_path):
    encodings = ['utf-8', 'latin-1', 'cp1252']
    for encoding in encodings:
        try:
            with open(file_path, 'r', encoding=encoding) as f:
                f.read()
            return encoding
        except UnicodeDecodeError:
            continue
    return 'utf-8'

def extract_description(code):
    comments = []
    for line in code.split('\n'):
        if line.strip().startswith('//'):
            clean_line = line.split('//')[1].strip()
            clean_line = clean_line.encode('ascii', 'ignore').decode()
            if clean_line:
                comments.append(clean_line)
    return ' '.join(comments[:5]) if comments else 'no_description'

def main():
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

    vectorizer = TfidfVectorizer(max_features=1000, min_df=2, max_df=0.95)
    tfidf_matrix = vectorizer.fit_transform(descriptions)

    data = {
        'vocab': vectorizer.vocabulary_,
        'idf': vectorizer.idf_.tolist(),
        'paths': paths,
        'descriptions': descriptions
    }

    # Ensure data directory exists
    os.makedirs('data', exist_ok=True)
    
    with open('data/processed_data.json', 'w', encoding='utf-8') as f:
        json.dump(data, f, ensure_ascii=False)
    print(f"Successfully processed {len(paths)} files")

if __name__ == "__main__":
    main()
