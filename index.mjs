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
    // Tokenize kode C++/Arduino dengan memisahkan berdasarkan spasi, operator, dan tanda baca
    const cleanedText = text.replace(/\/\/.*$/gm, '') // Hapus komentar satu baris
                            .replace(/\/\*[\s\S]*?\*\//g, '') // Hapus komentar multi-barisan
                            .replace(/\s+/g, ' ') // Ganti spasi ganda dengan satu spasi
                            .replace(/([{}();,=+\-*/<>_&|^%!~?])/g, ' $1 ') // Pisahkan operator dan tanda baca
                            .replace(/\s+/g, ' ') // Bersihkan spasi ganda
                            .trim(); 

    const tokens = cleanedText.split(' '); // Pisahkan berdasarkan spasi
    return tokens.filter(token => token.length > 0); // Hanya ambil token yang bukan spasi kosong
}

function createTrainingData(data) {
    const inputs = [];
    const outputs = [];
    const maxLength = 50;

    // Buat vocabulary dari semua token
    const allTokens = new Set();

    data.forEach(({ input, output }) => {
        const inputTokens = tokenize(input);
        const outputTokens = tokenize(output);

        // Tambahkan token ke vocabulary
        inputTokens.forEach(token => allTokens.add(token));
        outputTokens.forEach(token => allTokens.add(token));

        // Padding tokens
        const paddedInput = [...inputTokens, ...Array(maxLength - inputTokens.length).fill('<PAD>')].slice(0, maxLength);
        const paddedOutput = [...outputTokens, ...Array(maxLength - outputTokens.length).fill('<PAD>')].slice(0, maxLength);
        
        inputs.push(paddedInput);
        outputs.push(paddedOutput);
    });

    // Buat vocabulary dari token unik
    const vocab = { 
        vocabulary: Array.from(allTokens),
        total_tokens: allTokens.size
    };

    // Simpan vocab
    const modelDir = path.resolve('./ai_model');
    const vocabPath = path.join(modelDir, 'vocab.json');

    try {
        fs.writeFileSync(vocabPath, JSON.stringify(vocab, null, 2));
        console.log("Vocabulary created and saved. Total unique tokens:", allTokens.size);
    } catch (err) {
        console.error('Error saving vocab.json:', err);
    }

    // Proses one-hot encoding dengan mapping token ke indeks
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
        vocab: vocab // Return the vocab as well
    };
}

async function trainAndSaveModel(folderPath, outputModelPath) {
    const rawData = preprocessData(folderPath);
    const { inputs, outputs, vocab } = createTrainingData(rawData); // Get vocab here

    const model = tf.sequential();
    model.add(tf.layers.embedding({ inputDim: vocab.total_tokens, outputDim: 64, inputLength: 50 }));
    model.add(tf.layers.lstm({ units: 128, returnSequences: true }));
    model.add(tf.layers.dense({ units: vocab.total_tokens, activation: 'softmax' }));

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
