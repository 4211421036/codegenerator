import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

function preprocessData(folderPath) {
    const files = fs.readdirSync(folderPath);
    const data = [];

    files.forEach(file => {
        if (file.endsWith('.ino')) {
            const content = fs.readFileSync(path.join(folderPath, file), 'utf8');
            // Ambil hanya bagian kode yang relevan
            data.push({ input: file.replace('.ino', ''), output: content });
        }
    });
    return data;
}

function tokenize(text) {
    // Bersihkan karakter yang tidak perlu dan tokenize berdasarkan kata kunci
    const cleanedText = text.replace(/[^a-zA-Z0-9{};(),=+\-*/<>_]/g, ' ').toLowerCase();
    const tokens = cleanedText.split(/\s+/);
    return tokens.slice(0, 50); // Ambil 50 token pertama
}

function createTrainingData(data) {
    const inputs = [];
    const outputs = [];
    const maxLength = 50; // Maksimal panjang token per input

    const allTokens = new Set();

    data.forEach(({ input, output }) => {
        const inputTokens = tokenize(input);
        const outputTokens = tokenize(output);

        // Menambahkan token ke vocab
        inputTokens.forEach(token => allTokens.add(token));
        outputTokens.forEach(token => allTokens.add(token));

        // Padding tokens untuk input dan output
        const paddedInput = [...inputTokens, ...Array(maxLength - inputTokens.length).fill('<PAD>')].slice(0, maxLength);
        const paddedOutput = [...outputTokens, ...Array(maxLength - outputTokens.length).fill('<PAD>')].slice(0, maxLength);

        inputs.push(paddedInput);
        outputs.push(paddedOutput);
    });

    const vocab = {
        vocabulary: Array.from(allTokens),
        total_tokens: allTokens.size
    };

    // Menyimpan vocab
    const modelDir = path.resolve('./ai_model');
    const vocabPath = path.join(modelDir, 'vocab.json');
    try {
        fs.writeFileSync(vocabPath, JSON.stringify(vocab, null, 2));
        console.log("Vocabulary created and saved. Total unique tokens:", allTokens.size);
    } catch (err) {
        console.error('Error saving vocab.json:', err);
    }

    // Melakukan one-hot encoding untuk output
    const tokenToIndex = new Map(Array.from(allTokens).map((token, index) => [token, index]));
    const oneHotEncodedOutputs = outputs.map(output =>
        output.map(token => {
            const oneHot = Array(vocab.total_tokens).fill(0);
            const index = tokenToIndex.get(token) || 0;
            oneHot[index] = 1;
            return oneHot;
        })
    );

    return {
        inputs: tf.tensor2d(inputs.map(input => input.map(token => tokenToIndex.get(token) || 0)), [inputs.length, maxLength]),
        outputs: tf.tensor3d(oneHotEncodedOutputs, [outputs.length, maxLength, vocab.total_tokens]),
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
