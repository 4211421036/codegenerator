import os
import json
from sklearn.feature_extraction.text import TfidfVectorizer

def detect_encoding(file_path):
    encodings = ['utf-8', 'latin-1', 'cp1252']  # Common encodings for Arduino
    for encoding in encodings:
        try:
            with open(file_path, 'r', encoding=encoding) as f:
                f.read()
            return encoding
        except UnicodeDecodeError:
            continue
    return 'utf-8'  # Fallback encoding

def extract_description(code):
    # Handle empty comments and non-ASCII characters
    comments = []
    for line in code.split('\n'):
        if line.strip().startswith('//'):
            clean_line = line.split('//')[1].strip()
            clean_line = clean_line.encode('ascii', 'ignore').decode()
            if clean_line:
                comments.append(clean_line)
    return ' '.join(comments[:5]) if comments else 'no_description'

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

with open('data/processed_data.json', 'w', encoding='utf-8') as f:
    json.dump(data, f, ensure_ascii=False)
