import os
import json
import numpy as np
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity

def load_processed_data():
    with open('data/processed_data.json', 'r', encoding='utf-8') as f:
        return json.load(f)

def find_best_match(user_input, data):
    vectorizer = TfidfVectorizer(vocabulary=data['vocab'])
    vectorizer.idf_ = np.array(data['idf'])

    input_tfidf = vectorizer.transform([user_input])
    similarities = cosine_similarity(
        input_tfidf,
        vectorizer.transform(data['descriptions'])
    )
    return np.argmax(similarities)

def main():
    # Load user input
    with open('data/inputs/user_input.txt', 'r', encoding='utf-8') as f:
        user_input = f.read().strip()

    # Load processed data
    data = load_processed_data()

    # Find best match
    best_idx = find_best_match(user_input, data)

    # Generate output
    with open(data['paths'][best_idx], 'r', encoding='utf-8') as f:
        generated_code = f.read()

    # Save output to web/ for GitHub Pages
    os.makedirs('web/outputs', exist_ok=True)
    with open('web/outputs/generated_code.ino', 'w', encoding='utf-8') as f:
        f.write(generated_code)

    print("Code generated successfully!")

if __name__ == "__main__":
    main()
