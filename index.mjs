import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

// Function to preprocess data from .ino files
function preprocessData(folderPath) {
    const files = fs.readdirSync(folderPath);
    const data = [];
    const vocabulary = new Set();

    files.forEach(file => {
        if (file.endsWith('.ino')) {
            const content = fs.readFileSync(`${folderPath}/${file}`, 'utf8');
            // Extract tokens and update vocabulary
            const tokens = content.split(/\s+/); // Split by whitespace
            tokens.forEach(token => vocabulary.add(token)); // Add unique tokens to the vocabulary
            data.push({ input: file.replace('.ino', ''), output: tokens });
        }
    });

    return { data, vocabulary: Array.from(vocabulary) }; // Convert Set to Array
}

// Tokenize function to convert tokens into indices based on the vocabulary
function tokenize(tokens, vocabulary) {
    const tokenMap = new Map(vocabulary.map((word, index) => [word, index])); // Map token -> index
    return tokens.map(token => tokenMap.get(token) || 0); // Convert tokens to indices
}

// Function to create training data
function createTrainingData(data, vocabulary) {
    const inputs = [];
    const outputs = [];
    const maxLength = 50; // Fixed sequence length

    data.forEach(({ input, output }) => {
        // Tokenize and pad/truncate inputs and outputs
        const inputTokens = tokenize(input.split(/\s+/), vocabulary);
        const outputTokens = tokenize(output, vocabulary);

        const paddedInput = [...inputTokens, ...Array(maxLength - inputTokens.length).fill(0)].slice(0, maxLength);
        const paddedOutput = [...outputTokens, ...Array(maxLength - outputTokens.length).fill(0)].slice(0, maxLength);

        inputs.push(paddedInput);
        outputs.push(paddedOutput);
    });

    // One-hot encode the output tokens
    const numClasses = vocabulary.length; // Vocabulary size
    const oneHotEncodedOutputs = outputs.map(output =>
        output.map(token => {
            const oneHot = Array(numClasses).fill(0);
            oneHot[token] = 1;
            return oneHot;
        })
    );

    return {
        inputs: tf.tensor2d(inputs, [inputs.length, maxLength]),
        outputs: tf.tensor3d(oneHotEncodedOutputs, [outputs.length, maxLength, numClasses]),
    };
}

async function trainAndSaveModel(folderPath, outputModelPath) {
    const { data, vocabulary } = preprocessData(folderPath);
    const { inputs, outputs } = createTrainingData(data, vocabulary);

    const model = tf.sequential();
    model.add(tf.layers.embedding({ inputDim: vocabulary.length, outputDim: 64, inputLength: 50 }));
    model.add(tf.layers.lstm({ units: 128, returnSequences: true }));
    model.add(tf.layers.dense({ units: vocabulary.length, activation: 'softmax' }));

    model.compile({ loss: 'categoricalCrossentropy', optimizer: 'adam' });

    console.log('Training model...');
    await model.fit(inputs, outputs, {
        epochs: 10,
        batchSize: 16,
    });

    console.log('Saving model...');
    await model.save(`file://${outputModelPath}`);

    // Save vocabulary to a file
    fs.writeFileSync(path.join(outputModelPath, 'vocabulary.json'), JSON.stringify(vocabulary, null, 2));
    console.log('Vocabulary saved to', path.join(outputModelPath, 'vocabulary.json'));
    console.log('Model saved to', outputModelPath);
}

// Paths
const folderPath = './arduino_code'; // Path to folder containing .ino files
const outputModelPath = './ai_model'; // Path to save trained model

trainAndSaveModel(folderPath, outputModelPath);
