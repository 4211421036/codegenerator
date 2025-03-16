import sys
import pickle
import os
from sklearn.feature_extraction.text import CountVectorizer

def load_model(model_dir='../training/model'):
    try:
        with open(os.path.join(model_dir, 'trained_model.pkl'), 'rb') as f:
            model, vectorizer = pickle.load(f)
        return model, vectorizer
    except FileNotFoundError:
        print(f"Error: Model file not found at {os.path.join(model_dir, 'trained_model.pkl')}")
        print("Please run train_model.py first to train and save the model.")
        sys.exit(1)

def generate_code(description, model, vectorizer, template_dir='templates'):
    X = vectorizer.transform([description])
    predicted_label = model.predict(X)[0]
    with open(os.path.join(template_dir, 'template.ino'), 'r', encoding='utf-8') as f:
        template = f.read()
    generated_code = template.replace('// INSERT CODE HERE', predicted_label)
    return generated_code

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python generate_code.py <description>")
        sys.exit(1)
    
    description = sys.argv[1]
    model, vectorizer = load_model()
    code = generate_code(description, model, vectorizer)
    
    output_file = 'generated_code.ino'
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(code)
    print(f"Generated code saved to {output_file}")
