import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';

// Function to preprocess data from .ino files
function preprocessData(folderPath) {
    const files = fs.readdirSync(folderPath);
    const data = [];

    files.forEach(file => {
        if (file.endsWith('.ino')) {
            const content = fs.readFileSync(`${folderPath}/${file}`, 'utf8');
            data.push({ input: file.replace('.ino', ''), output: content });
        }
    });

    return data;
}

// Tokenize function with padding to ensure all token sequences are of equal length (50 tokens)
function tokenize(text) {
    return text.split(/\s+/).map((_, i) => i); // Simple tokenization
}

function createTrainingData(data) {
    const inputs = [];
    const outputs = [];

    const maxTokens = 50; // Define a fixed maximum length for tokens

    data.forEach(({ input, output }) => {
        const inputTokens = tokenize(input).slice(0, maxTokens);
        const outputTokens = tokenize(output).slice(0, maxTokens);

        // Pad or truncate to a fixed size (e.g., 50 tokens)
        const paddedInput = [...inputTokens, ...Array(maxTokens - inputTokens.length).fill(0)];
        const paddedOutput = [...outputTokens, ...Array(maxTokens - outputTokens.length).fill(0)];

        inputs.push(paddedInput);
        outputs.push(paddedOutput);
    });

    // One-hot encode the output tokens
    const numClasses = 1000; // Define the number of classes (size of vocabulary)
    const oneHotEncodedOutputs = outputs.map(output =>
        output.map(token => {
            const oneHot = Array(numClasses).fill(0);
            oneHot[token] = 1; // Set the index corresponding to the token to 1
            return oneHot;
        })
    );

    return {
        inputs: tf.tensor2d(inputs, [inputs.length, maxTokens]), // Ensure input tensor is of size [inputCount, maxTokens]
        outputs: tf.tensor3d(oneHotEncodedOutputs, [outputs.length, maxTokens, numClasses]), // One-hot encoded output
    };
}

async function trainAndSaveModel(folderPath, outputModelPath) {
    const rawData = preprocessData(folderPath);
    const { inputs, outputs } = createTrainingData(rawData);

    const model = tf.sequential();
    // Adjusted the embedding input length to match the fixed size (50 tokens)
    model.add(tf.layers.embedding({ inputDim: 1000, outputDim: 64, inputLength: 50 }));
    model.add(tf.layers.lstm({ units: 128, returnSequences: true }));
    model.add(tf.layers.dense({ units: 1000, activation: 'softmax' }));

    model.compile({ loss: 'categoricalCrossentropy', optimizer: 'adam' });

    console.log('Training model...');
    await model.fit(inputs, outputs, {
        epochs: 10,
        batchSize: 16,
    });

    console.log('Saving model...');
    await model.save(`file://${outputModelPath}`);
    console.log('Model saved to', outputModelPath);
}

const folderPath = './arduino_code'; // Ensure this folder exists in your repo
const outputModelPath = './ai_model'; // Store model in the current repository folder
trainAndSaveModel(folderPath, outputModelPath);
