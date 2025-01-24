import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';

// Function to preprocess data from .ino files
function preprocessData(folderPath) {
    const files = fs.readdirSync(folderPath);
    const data = [];
    const vocabSet = new Set();

    files.forEach(file => {
        if (file.endsWith('.ino')) {
            const content = fs.readFileSync(`${folderPath}/${file}`, 'utf8');
            const tokens = content.split(/\s+/);
            tokens.forEach(token => vocabSet.add(token)); // Add tokens to the vocabulary set
            data.push({ input: file.replace('.ino', ''), output: content });
        }
    });

    // Create vocabulary
    const vocab = Array.from(vocabSet);
    fs.writeFileSync('./ai_model/vocab.json', JSON.stringify(vocab, null, 2), 'utf8'); // Save vocab.json
    console.log('Vocabulary created and saved as vocab.json');

    return { data, vocab };
}

// Function to tokenize using the vocabulary
function tokenize(text, vocab) {
    const vocabMap = vocab.reduce((map, token, index) => {
        map[token] = index + 1; // Assign 1-based indices
        return map;
    }, {});

    return text.split(/\s+/).map(token => vocabMap[token] || 0); // Map tokens to indices
}

function createTrainingData(data, vocab) {
    const inputs = [];
    const outputs = [];

    const maxLength = 50; // Fixed sequence length

    data.forEach(({ input, output }) => {
        const inputTokens = tokenize(input, vocab);
        const outputTokens = tokenize(output, vocab);

        // Pad or truncate sequences
        const paddedInput = [...inputTokens, ...Array(maxLength - inputTokens.length).fill(0)].slice(0, maxLength);
        const paddedOutput = [...outputTokens, ...Array(maxLength - outputTokens.length).fill(0)].slice(0, maxLength);

        inputs.push(paddedInput);
        outputs.push(paddedOutput);
    });

    // Convert to tensors
    const inputTensor = tf.tensor2d(inputs, [inputs.length, maxLength]);
    const outputTensor = tf.oneHot(tf.tensor1d(outputs.flat(), 'int32'), vocab.length + 1).reshape([outputs.length, maxLength, vocab.length + 1]);

    return { inputs: inputTensor, outputs: outputTensor };
}

async function trainAndSaveModel(folderPath, outputModelPath) {
    const { data, vocab } = preprocessData(folderPath);
    const { inputs, outputs } = createTrainingData(data, vocab);

    const model = tf.sequential();
    model.add(tf.layers.embedding({ inputDim: vocab.length + 1, outputDim: 64, inputLength: 50 }));
    model.add(tf.layers.lstm({ units: 128, returnSequences: true }));
    model.add(tf.layers.dense({ units: vocab.length + 1, activation: 'softmax' }));

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

const folderPath = './arduino_code'; // Folder containing .ino files
const outputModelPath = './ai_model'; // Directory to save model
fs.mkdirSync(outputModelPath, { recursive: true }); // Ensure the output directory exists
trainAndSaveModel(folderPath, outputModelPath);
