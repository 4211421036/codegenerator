import os
import pickle
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.naive_bayes import MultinomialNB
import chardet  # Library untuk mendeteksi encoding file

# Fungsi untuk memuat data dari folder
def load_data(directory):
    data = []
    labels = []
    for filename in os.listdir(directory):
        filepath = os.path.join(directory, filename)
        try:
            with open(filepath, 'rb') as file:  # Baca file dalam mode binary
                raw_data = file.read()
                # Deteksi encoding file
                result = chardet.detect(raw_data)
                encoding = result['encoding']
                # Decode file dengan encoding yang terdeteksi
                content = raw_data.decode(encoding, errors='replace')
                data.append(content)
                labels.append(filename)
        except Exception as e:
            print(f"Error processing file {filename}: {e}")
    return data, labels

# Fungsi untuk melatih model
def train_model(data, labels):
    vectorizer = CountVectorizer()
    X = vectorizer.fit_transform(data)
    model = MultinomialNB()
    model.fit(X, labels)
    return model, vectorizer

# Fungsi untuk menyimpan model
def save_model(model, vectorizer, model_dir='model'):
    os.makedirs(model_dir, exist_ok=True)
    with open(os.path.join(model_dir, 'trained_model.pkl'), 'wb') as f:
        pickle.dump((model, vectorizer), f)

# Main function
if __name__ == "__main__":
    # Memuat data dari folder arduino_code
    data, labels = load_data('../arduino_code')
    # Melatih model
    model, vectorizer = train_model(data, labels)
    # Menyimpan model
    save_model(model, vectorizer)
    print("Model training completed and saved!")
