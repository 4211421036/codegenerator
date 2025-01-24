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
    const cleanedText = text.replace(/\/\/.*$/gm, '')               // Hapus komentar satu baris
                            .replace(/\/\*[\s\S]*?\*\//g, '')      // Hapus komentar blok
                            .replace(/\s+/g, ' ')                  // Gabungkan spasi
                            .replace(/([{}();,=+\-*/<>_&|^%!~?])/g, ' $1 ')  // Beri spasi di sekitar simbol
                            .replace(/\s+/g, ' ')                  // Bersihkan spasi ekstra
                            .trim();                               // Hapus spasi di awal dan akhir

    const tokens = cleanedText.split(' '); // Pisahkan berdasarkan spasi
    return tokens.filter(token => token.length > 0); // Hapus token kosong
}

function createTrainingData(data) {
    const inputs = [];
    const outputs = [];
    const maxLength = 50;

    const allTokens = new Set();

    data.forEach(({ input, output }) => {
        const inputTokens = tokenize(input);
        const outputTokens = tokenize(output);

        inputTokens.forEach(token => allTokens.add(token));
        outputTokens.forEach(token => allTokens.add(token));

        const paddedInput = [...inputTokens, ...Array(maxLength - inputTokens.length).fill('<PAD>')].slice(0, maxLength);
        const paddedOutput = outputTokens.length < maxLength
            ? [...outputTokens, ...Array(maxLength - outputTokens.length).fill('<PAD>')].slice(0, maxLength)
            : outputTokens.slice(0, maxLength);

        inputs.push(paddedInput);
        outputs.push(paddedOutput);
    });

    const vocab = {
        vocabulary: Array.from(allTokens),
        total_tokens: allTokens.size
    };

    const vocabPath = path.join('./ai_model', 'vocab.json');

    try {
        fs.writeFileSync(vocabPath, JSON.stringify(vocab, null, 2));
        console.log("Vocabulary created and saved.");
    } catch (err) {
        console.error('Error saving vocab.json:', err);
    }

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
        inputs: tf.tensor2d(
            inputs.map(input => input.map(token => tokenToIndex.get(token) || 0)),
            [inputs.length, maxLength]
        ),
        outputs: tf.tensor3d(oneHotEncodedOutputs, [outputs.length, maxLength, vocab.total_tokens]),
        vocab: vocab
    };
}

function exactMatchLoss(yTrue, yPred) {
    // Loss untuk matching yang lebih baik, menggunakan binary cross entropy
    return tf.losses.binaryCrossentropy(yTrue, yPred);
}

async function trainAndSaveModel(folderPath, outputModelPath) {
    const rawData = preprocessData(folderPath);
    const { inputs, outputs, vocab } = createTrainingData(rawData);

    const model = tf.sequential();

    model.add(tf.layers.embedding({
        inputDim: vocab.total_tokens,
        outputDim: 64,
        inputLength: 50
    }));
    model.add(tf.layers.lstm({
        units: 256,
        returnSequences: true,
        inputShape: [50, vocab.total_tokens]
    }));
    model.add(tf.layers.lstm({ units: 128, returnSequences: true }));
    model.add(tf.layers.dense({ units: 1000, activation: 'softmax' }));
    model.add(tf.layers.lstm({ units: 128, returnSequences: true }));
    model.add(tf.layers.dense({ units: vocab.total_tokens, activation: 'softmax' }));

    // Compile model with the custom exactMatchLoss function
    model.compile({
        loss: exactMatchLoss,
        optimizer: tf.train.adam(0.001)
    });

    console.log('Training model...');
    await model.fit(inputs, outputs, {
        epochs: 20,
        batchSize: 32,
    });

    console.log('Saving model...');
    await model.save(`file://${outputModelPath}`);
    console.log('Model saved to', outputModelPath);
}

const folderPath = './arduino_code'; // Ganti dengan folder yang sesuai
const outputModelPath = './ai_model'; // Ganti dengan path tujuan model
trainAndSaveModel(folderPath, outputModelPath);
