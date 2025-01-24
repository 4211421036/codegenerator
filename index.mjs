import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

function preprocessData(folderPath) {
    const files = fs.readdirSync(folderPath);
    const data = [];

    files.forEach(file => {
        if (file.endsWith('.ino')) {
            const content = fs.readFileSync(path.join(folderPath, file), 'utf8');
            // Preserve code structure
            data.push({ 
                input: file.replace('.ino', ''), 
                output: content.trim()
            });
        }
    });
    return data;
}

function advancedTokenize(text) {
    // Preserve code structure and meaningful tokens
    return text
        .split(/\n/)
        .map(line => line.trim())
        .filter(line => line.length > 0 && !line.startsWith('//'));
}

function createTrainingData(data) {
    const maxLength = 100;  // Increased to capture more context
    const inputs = [];
    const outputs = [];
    const allTokens = new Set();

    data.forEach(({ input, output }) => {
        const inputTokens = advancedTokenize(input);
        const outputTokens = advancedTokenize(output);

        inputTokens.forEach(token => allTokens.add(token));
        outputTokens.forEach(token => allTokens.add(token));

        // Pad or truncate tokens while maintaining structure
        const paddedInput = inputTokens.slice(0, maxLength);
        const paddedOutput = outputTokens.slice(0, maxLength);

        inputs.push(paddedInput);
        outputs.push(paddedOutput);
    });

    const vocab = {
        vocabulary: Array.from(allTokens),
        total_tokens: allTokens.size
    };

    const tokenToIndex = new Map(
        Array.from(allTokens).map((token, index) => [token, index])
    );

    return {
        inputs: tf.tensor2d(
            inputs.map(input => input.map(token => tokenToIndex.get(token) || 0)),
            [inputs.length, maxLength]
        ),
        outputs: tf.tensor2d(
            outputs.map(output => output.map(token => tokenToIndex.get(token) || 0)),
            [outputs.length, maxLength]
        ),
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
        epochs: 50,
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
