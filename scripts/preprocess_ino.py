import os
import json
from sklearn.feature_extraction.text import TfidfVectorizer

def extract_description(code):
    return ' '.join([line.split('//')[1].strip() 
                   for line in code.split('\n') 
                   if line.strip().startswith('//')][:5])

descriptions, paths = [], []
for root, _, files in os.walk('arduino_code'):
    for file in files:
        if file.endswith('.ino'):
            path = os.path.join(root, file)
            with open(path) as f:
                code = f.read()
            desc = extract_description(code)
            descriptions.append(desc)
            paths.append(path)

vectorizer = TfidfVectorizer(max_features=1000)
tfidf_matrix = vectorizer.fit_transform(descriptions)

data = {
    'vocab': vectorizer.vocabulary_,
    'idf': vectorizer.idf_.tolist(),
    'paths': paths,
    'descriptions': descriptions
}

with open('data/processed_data.json', 'w') as f:
    json.dump(data, f)
