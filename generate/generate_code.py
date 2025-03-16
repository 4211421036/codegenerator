import sys
import pickle
import os
from sklearn.feature_extraction.text import CountVectorizer

def load_model(model_dir='model'):
    with open(os.path.join(model_dir, 'trained_model.pkl'), 'rb') as f:
        model, vectorizer = pickle.load(f)
    return model, vectorizer

def generate_code(description, model, vectorizer, template_dir='templates'):
    X = vectorizer.transform([description])
    predicted_label = model.predict(X)[0]
    with open(os.path.join(template_dir, 'template.ino'), 'r', encoding='utf-8') as f:
        template = f.read()
    generated_code = template.replace('// INSERT CODE HERE', predicted_label)
    return generated_code

if __name__ == "__main__":
    # Baca deskripsi dari argumen command-line
    if len(sys.argv) < 2:
        print("Usage: python generate_code.py <description>")
        sys.exit(1)
    description = sys.argv[1]
    
    # Memuat model
    model, vectorizer = load_model()
    
    # Menghasilkan kode
    code = generate_code(description, model, vectorizer)
    
    # Menyimpan kode yang dihasilkan ke file
    output_file = 'generated_code.ino'
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(code)
    print(f"Generated code saved to {output_file}")
