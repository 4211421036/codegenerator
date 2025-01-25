import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

function preprocessData(folderPath) {
    const files = fs.readdirSync(folderPath);
    return files
        .filter(file => file.endsWith('.ino'))
        .map(file => ({
            input: file.replace('.ino', ''),
            output: fs.readFileSync(path.join(folderPath, file), 'utf8').trim(),
        }));
}

function advancedTokenize(text, maxLength = 100) {
    return text
        .toLowerCase()
        .replace(/[^\w\s]/g, '')
        .split(/\s+/)
        .filter(Boolean)
        .slice(0, maxLength);
}

function createTrainingData(data) {
    const maxLength = 150;
    const tokensSet = new Set(['<PAD>', '<START>', '<END>']);
    data.forEach(({ input, output }) => {
        advancedTokenize(input).forEach(token => tokensSet.add(token));
        advancedTokenize(output).forEach(token => tokensSet.add(token));
    });

    const tokenToIndex = Object.fromEntries(
        Array.from(tokensSet).map((token, index) => [token, index])
    );

    const inputs = [];
    const outputs = [];

    data.forEach(({ input, output }) => {
        const inputTokens = ['<START>', ...advancedTokenize(input, maxLength - 2), '<END>'];
        const outputTokens = ['<START>', ...advancedTokenize(output, maxLength - 2), '<END>'];

        const pad = (tokens) =>
            [...tokens, ...Array(maxLength - tokens.length).fill('<PAD>')].slice(0, maxLength);

        inputs.push(pad(inputTokens).map(token => tokenToIndex[token] || 0));
        outputs.push(
            pad(outputTokens).map(token => {
                const oneHot = Array(tokensSet.size).fill(0);
                oneHot[tokenToIndex[token] || 0] = 1;
                return oneHot;
            })
        );
    });

    return {
        inputs: tf.tensor2d(inputs, [inputs.length, maxLength]),
        outputs: tf.tensor3d(outputs, [outputs.length, maxLength, tokensSet.size]),
        vocab: { vocabulary: Array.from(tokensSet), tokenToIndex },
    };
}

async function trainModel(dataPath, outputPath) {
    const data = preprocessData(dataPath);
    const { inputs, outputs, vocab } = createTrainingData(data);

    const model = tf.sequential();
    model.add(tf.layers.embedding({ inputDim: vocab.vocabulary.length, outputDim: 256, inputLength: 150 }));
    model.add(tf.layers.lstm({ units: 512, returnSequences: true, recurrentDropout: 0.2 }));
    model.add(tf.layers.dropout({ rate: 0.3 }));
    model.add(tf.layers.lstm({ units: 512, returnSequences: true, recurrentDropout: 0.2 }));
    model.add(tf.layers.dense({ units: vocab.vocabulary.length, activation: 'softmax' }));

    model.compile({
        loss: 'categoricalCrossentropy',
        optimizer: tf.train.adam(0.001),
        metrics: ['accuracy'],
    });

    console.log('Training...');
    await model.fit(inputs, outputs, {
        epochs: 120,
        batchSize: 32,
        validationSplit: 0.2,
    });

    await model.save(`file://${outputPath}`);
    fs.writeFileSync(path.join(outputPath, 'vocab.json'), JSON.stringify(vocab, null, 2));
    console.log('Model trained and saved!');
}

async function generateArduinoCode(model, vocab, inputDescription) {
    const inputTokens = advancedTokenize(inputDescription, 150);
    const tokenToIndex = vocab.tokenToIndex;
    const paddedInput = [
        tokenToIndex['<START>'],
        ...inputTokens.map((token) => tokenToIndex[token] || tokenToIndex['<PAD>']),
        tokenToIndex['<END>'],
    ].slice(0, 150);

    const inputTensor = tf.tensor2d([paddedInput], [1, 150]);
    const prediction = model.predict(inputTensor).arraySync()[0];

    const outputTokens = prediction
        .map((prob, index) => ({ token: vocab.vocabulary[index], prob }))
        .filter(({ token }) => token !== '<PAD>' && token !== '<START>' && token !== '<END>')
        .sort((a, b) => b.prob - a.prob)
        .map(({ token }) => token);

    return `void setup() {\n  ${outputTokens.slice(0, 10).join(' ')}\n}\n\nvoid loop() {\n  ${outputTokens.slice(10).join(' ')}\n}`;
}

async function main() {
    const folderPath = './arduino_code';
    const outputModelPath = './ai_model';

    await trainModel(folderPath, outputModelPath);

    const model = await tf.loadLayersModel(`file://${outputModelPath}/model.json`);
    const vocab = JSON.parse(fs.readFileSync(path.join(outputModelPath, 'vocab.json'), 'utf8'));

    const userDescription = 'Temperature sensor with LCD display';
    const arduinoCode = await generateArduinoCode(model, vocab, userDescription);
    console.log(arduinoCode);
}

main().catch(console.error);
