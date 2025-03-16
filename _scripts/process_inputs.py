import os
import json
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity

def load_processed_data():
    with open('_data/processed_data.json') as f:
        return json.load(f)

def process_user_input():
    with open('_site/inputs/user_input.txt') as f:
        return f.read().strip()

def generate_code(user_input, data):
    vectorizer = TfidfVectorizer(vocabulary=data['vocab'])
    vectorizer.idf_ = data['idf']
    
    input_tfidf = vectorizer.transform([user_input])
    similarities = cosine_similarity(
        input_tfidf, 
        vectorizer.transform(data['descriptions'])
    )
    best_idx = similarities.argmax()
    
    with open(data['paths'][best_idx]) as f:
        return f.read()

def main():
    data = load_processed_data()
    user_input = process_user_input()
    code = generate_code(user_input, data)
    
    with open('_site/outputs/generated_code.ino', 'w') as f:
        f.write(code)

if __name__ == "__main__":
    main()
