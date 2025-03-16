import pickle
import os
from sklearn.feature_extraction.text import CountVectorizer

# Load model
def load_model(model_dir='model'):
    with open(os.path.join(model_dir, 'trained_model.pkl'), 'rb') as f:
        model, vectorizer = pickle.load(f)
    return model, vectorizer

# Generate code
def generate_code(description, model, vectorizer, template_dir='templates'):
    X = vectorizer.transform([description])
    predicted_label = model.predict(X)[0]
    with open(os.path.join(template_dir, 'template.ino'), 'r') as f:
        template = f.read()
    # Replace placeholders in template with predicted values
    generated_code = template.replace('// INSERT CODE HERE', predicted_label)
    return generated_code

if __name__ == "__main__":
    description = input("Enter description: ")
    model, vectorizer = load_model()
    code = generate_code(description, model, vectorizer)
    print(code)
