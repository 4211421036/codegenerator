import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

function preprocessData(folderPath) {
    const files = fs.readdirSync(folderPath);
    const data = [];
    files.forEach(file => {
        if (file.endsWith('.ino')) {
            const content = fs.readFileSync(path.join(folderPath, file), 'utf8');
            data.push({ input: file.replace('.ino', ''), output: content });
        }
    });
    return data;
}

function tokenize(text) {
    return text.split(/\s+/).map((_, i) => i).slice(0, 50);
}

function createTrainingData(data) {
    const inputs = [];
    const outputs = [];
    const maxLength = 50;
    const numClasses = 1000;

    data.forEach(({ input, output }) => {
        const inputTokens = tokenize(input);
        const outputTokens = tokenize(output);

        const paddedInput = [...inputTokens, ...Array(maxLength - inputTokens.length).fill(0)].slice(0, maxLength);
        const paddedOutput = [...outputTokens, ...Array(maxLength - outputTokens.length).fill(0)].slice(0, maxLength);
        
        inputs.push(paddedInput);
        outputs.push(paddedOutput);
    });

    const flattenedInputs = inputs.reduce((acc, curr) => acc.concat(curr), []);
    const vocab = { vocabulary: Array.from(new Set(flattenedInputs)) };
    
    // Ensure ai_model directory exists
    const modelDir = './ai_model';
    if (!fs.existsSync(modelDir)){
        fs.mkdirSync(modelDir);
    }

    // Save vocabulary to file in ai_model directory
    const vocabPath = path.join(modelDir, 'vocab.json');
    fs.writeFileSync(vocabPath, JSON.stringify(vocab));
    console.log("Vocabulary created and saved as vocab.json");

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
    const rawData = preprocessData(folderPath);
    const { inputs, outputs } = createTrainingData(rawData);

    const model = tf.sequential();
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

const folderPath = './arduino_code';
const outputModelPath = './ai_model';
trainAndSaveModel(folderPath, outputModelPath);
