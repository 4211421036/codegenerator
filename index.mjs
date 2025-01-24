import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';

// Function to preprocess data from .ino files and build vocabulary
function preprocessDataAndBuildVocab(folderPath) {
    const files = fs.readdirSync(folderPath);
    const data = [];
    const vocabSet = new Set();

    files.forEach(file => {
        if (file.endsWith('.ino')) {
            const content = fs.readFileSync(`${folderPath}/${file}`, 'utf8');
            const tokens = content.split(/\s+/); // Split by whitespace
            tokens.forEach(token => vocabSet.add(token)); // Add tokens to vocabulary
            data.push({ input: file.replace('.ino', ''), output: tokens });
        }
    });

    const vocabulary = Array.from(vocabSet); // Convert Set to Array
    const vocabIndex = {};
    vocabulary.forEach((word, index) => {
        vocabIndex[word] = index; // Map each token to an index
    });

    return { data, vocabulary, vocabIndex };
}

// Function to tokenize text based on vocabulary
function tokenize(text, vocabIndex, maxLength = 50) {
    const tokens = text.map(word => vocabIndex[word] || 0); // Map tokens to indices
    return [...tokens, ...Array(maxLength - tokens.length).fill(0)].slice(0, maxLength); // Pad or truncate
}

// Function to create training data
function createTrainingData(data, vocabIndex, maxLength = 50) {
    const inputs = [];
    const outputs = [];

    data.forEach(({ input, output }) => {
        const inputTokens = tokenize(input.split(/\s+/), vocabIndex, maxLength);
        const outputTokens = tokenize(output, vocabIndex, maxLength);

        inputs.push(inputTokens);
        outputs.push(outputTokens);
    });

    return {
        inputs: tf.tensor2d(inputs, [inputs.length, maxLength]),
        outputs: tf.tensor2d(outputs, [outputs.length, maxLength]),
    };
}

// Function to train and save the model
async function trainAndSaveModel(folderPath, outputModelPath) {
    const { data, vocabulary, vocabIndex } = preprocessDataAndBuildVocab(folderPath);
    const { inputs, outputs } = createTrainingData(data, vocabIndex);

    const model = tf.sequential();
    model.add(tf.layers.embedding({ inputDim: vocabulary.length, outputDim: 64, inputLength: 50 }));
    model.add(tf.layers.lstm({ units: 128, returnSequences: true }));
    model.add(tf.layers.dense({ units: vocabulary.length, activation: 'softmax' }));

    model.compile({ loss: 'sparseCategoricalCrossentropy', optimizer: 'adam' });

    console.log('Training model...');
    await model.fit(inputs, outputs, {
        epochs: 10,
        batchSize: 16,
    });

    console.log('Saving model...');
    await model.save(`file://${outputModelPath}`);
    console.log('Model saved to', outputModelPath);

    // Save the vocabulary to a file
    fs.writeFileSync(`${outputModelPath}/vocab.json`, JSON.stringify(vocabulary));
}

// Function to generate code using the trained model
async function generateCode(modelPath, vocabPath, inputText, maxLength = 50) {
    const model = await tf.loadLayersModel(`file://${modelPath}/model.json`);
    const vocabulary = JSON.parse(fs.readFileSync(vocabPath, 'utf8'));
    const vocabIndex = {};
    vocabulary.forEach((word, index) => {
        vocabIndex[word] = index;
    });

    const inputTokens = tokenize(inputText.split(/\s+/), vocabIndex, maxLength);
    const inputTensor = tf.tensor2d([inputTokens], [1, maxLength]);

    const output = model.predict(inputTensor).arraySync()[0];
    const result = output.map(indices => vocabulary[indices]);

    return result.join(' ');
}

// Paths
const folderPath = './arduino_code'; // Folder containing .ino files
const outputModelPath = './ai_model'; // Folder to save the model
const vocabPath = `${outputModelPath}/vocab.json`;

// Train the model
trainAndSaveModel(folderPath, outputModelPath)
    .then(() => {
        console.log('Training complete.');
    })
    .catch(err => {
        console.error('Error during training:', err);
    });
