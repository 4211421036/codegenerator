import os
import pickle
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.naive_bayes import MultinomialNB

# Load and preprocess data
def load_data(directory):
    data = []
    labels = []
    for filename in os.listdir(directory):
        with open(os.path.join(directory, filename), 'r') as file:
            data.append(file.read())
            labels.append(filename)
    return data, labels

# Train model
def train_model(data, labels):
    vectorizer = CountVectorizer()
    X = vectorizer.fit_transform(data)
    model = MultinomialNB()
    model.fit(X, labels)
    return model, vectorizer

# Save model
def save_model(model, vectorizer, model_dir='model'):
    os.makedirs(model_dir, exist_ok=True)
    with open(os.path.join(model_dir, 'trained_model.pkl'), 'wb') as f:
        pickle.dump((model, vectorizer), f)

if __name__ == "__main__":
    data, labels = load_data('../arduino_code')
    model, vectorizer = train_model(data, labels)
    save_model(model, vectorizer)
