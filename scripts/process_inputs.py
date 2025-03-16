import os
import json
import numpy as np
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity

with open('data/processed_data.json') as f:
    data = json.load(f)

vectorizer = TfidfVectorizer(vocabulary=data['vocab'])
vectorizer.idf_ = np.array(data['idf'])

for input_file in os.listdir('data/inputs'):
    if input_file.endswith('.txt'):
        with open(f'data/inputs/{input_file}') as f:
            user_input = f.read()
        
        input_tfidf = vectorizer.transform([user_input])
        similarities = cosine_similarity(
            input_tfidf, 
            vectorizer.transform(data['descriptions'])
        )
        best_idx = np.argmax(similarities)
        
        with open(data['paths'][best_idx]) as f:
            generated_code = f.read()
        
        output_file = input_file.replace('.txt', '.ino')
        with open(f'data/outputs/{output_file}', 'w') as f:
            f.write(generated_code)
