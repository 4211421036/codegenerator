import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

function preprocessData(folderPath) {
    const files = fs.readdirSync(folderPath);
    const data = [];

    files.forEach(file => {
        if (file.endsWith('.ino')) {
            const content = fs.readFileSync(path.join(folderPath, file), 'utf8');
            data.push({ 
                input: file.replace('.ino', ''), 
                output: content.trim()
            });
        }
    });
    return data;
}

function advancedTokenize(text, maxLength = 100) {
    return text
        .split(/\n/)
        .map(line => line.trim())
        .filter(line => line.length > 0 && !line.startsWith('//'))
        .slice(0, maxLength);
}

function createTrainingData(data) {
    const maxLength = 100;
    const allTokens = new Set(['<PAD>']);

    // Collect all unique tokens
    data.forEach(({ input, output }) => {
        const inputTokens = advancedTokenize(input);
        const outputTokens = advancedTokenize(output);

        inputTokens.forEach(token => allTokens.add(token));
        outputTokens.forEach(token => allTokens.add(token));
    });

    const tokenToIndex = new Map(
        Array.from(allTokens).map((token, index) => [token, index])
    );

    const inputs = [];
    const outputs = [];

    data.forEach(({ input, output }) => {
        // Tokenize and pad input
        const inputTokens = advancedTokenize(input, maxLength);
        const paddedInput = [
            ...inputTokens, 
            ...Array(maxLength - inputTokens.length).fill('<PAD>')
        ].slice(0, maxLength);

        // Tokenize and pad output
        const outputTokens = advancedTokenize(output, maxLength);
        const paddedOutput = [
            ...outputTokens, 
            ...Array(maxLength - outputTokens.length).fill('<PAD>')
        ].slice(0, maxLength);

        // Create one-hot encoded output
        const oneHotOutput = paddedOutput.map(token => {
            const oneHot = new Array(allTokens.size).fill(0);
            const index = tokenToIndex.get(token);
            oneHot[index] = 1;
            return oneHot;
        });

        inputs.push(paddedInput.map(token => tokenToIndex.get(token) || 0));
        outputs.push(oneHotOutput);
    });

    const vocab = {
        vocabulary: Array.from(allTokens),
        total_tokens: allTokens.size
    };

    return {
        inputs: tf.tensor2d(inputs, [inputs.length, maxLength]),
        outputs: tf.tensor3d(outputs, [outputs.length, maxLength, allTokens.size]),
        vocab: vocab
    };
}

async function trainAndSaveModel(folderPath, outputModelPath) {
    const rawData = preprocessData(folderPath);
    const { inputs, outputs, vocab } = createTrainingData(rawData);

    const model = tf.sequential();

    model.add(tf.layers.embedding({
        inputDim: vocab.total_tokens,
        outputDim: 128,
        inputLength: 100
    }));
    
    model.add(tf.layers.lstm({
        units: 256, 
        returnSequences: true
    }));
    
    model.add(tf.layers.dropout({ rate: 0.3 }));
    
    model.add(tf.layers.lstm({ 
        units: 256, 
        returnSequences: true 
    }));
    
    model.add(tf.layers.dense({ 
        units: vocab.total_tokens, 
        activation: 'softmax' 
    }));
    
    model.compile({
        loss: 'categoricalCrossentropy',
        optimizer: tf.train.adam(0.001)
    });

    console.log('Training model...');
    await model.fit(inputs, outputs, {
        epochs: 50000,
        batchSize: 32,
        validationSplit: 0.2
    });

    console.log('Saving model...');
    await model.save(`file://${outputModelPath}`);
    console.log('Model saved.');

    // Save vocabulary
    fs.writeFileSync(
        path.join(outputModelPath, 'vocab.json'), 
        JSON.stringify(vocab, null, 2)
    );
}

const folderPath = './arduino_code';
const outputModelPath = './ai_model';
trainAndSaveModel(folderPath, outputModelPath);
