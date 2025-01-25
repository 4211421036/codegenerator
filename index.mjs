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
    // More sophisticated tokenization
    return text
        .toLowerCase()
        .replace(/[^\w\s]/g, '')
        .split(/\s+/)
        .filter(token => token.length > 0)
        .slice(0, maxLength);
}

function createTrainingData(data) {
    const maxLength = 150;  // Increased max length
    const allTokens = new Set(['<PAD>', '<START>', '<END>']);

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
        // Enhanced tokenization and padding
        const inputTokens = ['<START>', ...advancedTokenize(input, maxLength-2), '<END>'];
        const outputTokens = ['<START>', ...advancedTokenize(output, maxLength-2), '<END>'];

        const paddedInput = [
            ...inputTokens, 
            ...Array(maxLength - inputTokens.length).fill('<PAD>')
        ].slice(0, maxLength);

        const paddedOutput = [
            ...outputTokens, 
            ...Array(maxLength - outputTokens.length).fill('<PAD>')
        ].slice(0, maxLength);

        // One-hot encoding with improved handling
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
        total_tokens: allTokens.size,
        tokenToIndex: Object.fromEntries(tokenToIndex)
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

    // Enhanced model architecture
    model.add(tf.layers.embedding({
        inputDim: vocab.total_tokens,
        outputDim: 256,
        inputLength: 150
    }));
    
    model.add(tf.layers.lstm({
        units: 512, 
        returnSequences: true,
        recurrentDropout: 0.2
    }));
    
    model.add(tf.layers.dropout({ rate: 0.3 }));
    
    model.add(tf.layers.lstm({ 
        units: 512, 
        returnSequences: true,
        recurrentDropout: 0.2
    }));
    
    model.add(tf.layers.dense({ 
        units: vocab.total_tokens, 
        activation: 'softmax' 
    }));
    
    model.compile({
        loss: 'categoricalCrossentropy',
        optimizer: tf.train.adam(0.001),
        metrics: ['accuracy']
    });

    console.log('Training model...');
    const history = await model.fit(inputs, outputs, {
        epochs: 500,
        batchSize: 64,
        validationSplit: 0.2,
        callbacks: {
            onEpochEnd: async (epoch, logs) => {
                console.log(`Epoch ${epoch}: loss = ${logs.loss}, accuracy = ${logs.accuracy}`);
            }
        }
    });

    console.log('Saving model...');
    await model.save(`file://${outputModelPath}`);
    console.log('Model saved.');

    // Save vocabulary with more details
    fs.writeFileSync(
        path.join(outputModelPath, 'vocab.json'), 
        JSON.stringify(vocab, null, 2)
    );

    return { model, vocab };
}

// Code generation function
function generateArduinoCode(model, vocab, inputDescription) {
    // Tokenize input
    const inputTokens = advancedTokenize(inputDescription);
    const maxLength = 150;

    // Prepare input tensor
    const tokenToIndex = vocab.tokenToIndex;
    const paddedInput = [
        tokenToIndex['<START>'],
        ...inputTokens.map(token => tokenToIndex[token] || tokenToIndex['<PAD>']),
        tokenToIndex['<END>'],
        ...Array(maxLength - inputTokens.length - 2).fill(tokenToIndex['<PAD>'])
    ].slice(0, maxLength);

    const inputTensor = tf.tensor2d([paddedInput], [1, maxLength]);

    // Generate prediction
    const prediction = model.predict(inputTensor);
    
    // Decode output
    const outputIndices = prediction.argMax(-1).dataSync();
    const generatedTokens = outputIndices
        .map(index => vocab.vocabulary[index])
        .filter(token => token && token !== '<PAD>' && token !== '<START>' && token !== '<END>');

    // Basic code template generation
    return `// Generated Arduino Code for: ${inputDescription}
void setup() {
    // Initialization
    ${generatedTokens.slice(0, 10).join(' ')}
}

void loop() {
    // Main logic
    ${generatedTokens.slice(10, 20).join(' ')}
}`;
}

const folderPath = './arduino_code';
const outputModelPath = './ai_model';

async function main() {
    const { model, vocab } = await trainAndSaveModel(folderPath, outputModelPath);
    
    // Example usage
    const projectDescription = "Temperature sensor with LCD display";
    const generatedCode = generateArduinoCode(model, vocab, projectDescription);
    console.log(generatedCode);
}

main().catch(console.error);
