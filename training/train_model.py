import os
import pickle
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.naive_bayes import MultinomialNB
import chardet

def load_data(directory):
    data = []
    labels = []
    for filename in os.listdir(directory):
        if not filename.endswith('.ino'):  # Hanya proses file .ino
            print(f"Skipping non-ino file: {filename}")
            continue
        filepath = os.path.join(directory, filename)
        try:
            with open(filepath, 'rb') as file:
                raw_data = file.read()
                result = chardet.detect(raw_data)
                encoding = result['encoding']
                if encoding is None:  # Jika encoding tidak terdeteksi, gunakan default
                    encoding = 'utf-8'
                content = raw_data.decode(encoding, errors='replace')
                data.append(content)
                labels.append(filename)
        except Exception as e:
            print(f"Error processing file {filename}: {e}")
    return data, labels

def train_model(data, labels):
    vectorizer = CountVectorizer()
    X = vectorizer.fit_transform(data)
    model = MultinomialNB()
    model.fit(X, labels)
    return model, vectorizer

def save_model(model, vectorizer, model_dir='model'):
    # Buat folder 'model' jika belum ada
    os.makedirs(model_dir, exist_ok=True)
    # Simpan model dan vectorizer ke file
    with open(os.path.join(model_dir, 'trained_model.pkl'), 'wb') as f:
        pickle.dump((model, vectorizer), f)
    print(f"Model saved to {os.path.join(model_dir, 'trained_model.pkl')}")

if __name__ == "__main__":
    # Memuat data dari folder arduino_code
    data, labels = load_data('../arduino_code')
    
    # Melatih model
    if not data:
        print("No valid .ino files found in the arduino_code folder.")
    else:
        model, vectorizer = train_model(data, labels)
        
        # Menyimpan model
        save_model(model, vectorizer)
        print("Model training completed and saved!")
