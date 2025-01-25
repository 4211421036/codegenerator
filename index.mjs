import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

class ArduinoCodeTrainer {
    constructor(folderPath) {
        this.folderPath = folderPath;
        this.maxLength = 150;
    }

    preprocessData() {
        const files = fs.readdirSync(this.folderPath);
        const data = [];

        files.forEach(file => {
            if (file.endsWith('.ino')) {
                const content = fs.readFileSync(path.join(this.folderPath, file), 'utf8');
                data.push({
                    input: file.replace('.ino', ''),
                    output: content.trim()
                });
            }
        });
        return data;
    }

    tokenize(text, maxLength = this.maxLength) {
        return text
            .toLowerCase()
            .replace(/[^\w\s]/g, '')
            .split(/\s+/)
            .filter(token => token.length > 0)
            .slice(0, maxLength);
    }

    createTrainingData(data) {
        // Limit the number of files to process to prevent array overflow
        const limitedData = data.slice(0, 50);
        
        const allTokens = new Set(['<PAD>', '<START>', '<END>']);

        // Collect all unique tokens
        limitedData.forEach(({ input, output }) => {
            const inputTokens = this.tokenize(input);
            const outputTokens = this.tokenize(output);

            inputTokens.forEach(token => allTokens.add(token));
            outputTokens.forEach(token => allTokens.add(token));
        });

        const tokenToIndex = new Map(
            Array.from(allTokens).map((token, index) => [token, index])
        );

        const inputs = [];
        const outputs = [];

        limitedData.forEach(({ input, output }) => {
            const inputTokens = ['<START>', ...this.tokenize(input), '<END>'];
            const outputTokens = ['<START>', ...this.tokenize(output), '<END>'];

            const paddedInput = [
                ...inputTokens,
                ...Array(this.maxLength - inputTokens.length).fill('<PAD>')
            ].slice(0, this.maxLength);

            const paddedOutput = [
                ...outputTokens,
                ...Array(this.maxLength - outputTokens.length).fill('<PAD>')
            ].slice(0, this.maxLength);

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
            inputs: tf.tensor2d(inputs, [inputs.length, this.maxLength]),
            outputs: tf.tensor3d(outputs, [outputs.length, this.maxLength, allTokens.size]),
            vocab: vocab
        };
    }

    createModel(vocabSize) {
        const model = tf.sequential();

        model.add(tf.layers.embedding({
            inputDim: vocabSize,
            outputDim: 256,
            inputLength: this.maxLength
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
            units: vocabSize,
            activation: 'softmax'
        }));

        model.compile({
            loss: 'categoricalCrossentropy',
            optimizer: tf.train.adam(0.001),
            metrics: ['accuracy']
        });

        return model;
    }

    async train() {
        const rawData = this.preprocessData();
        const { inputs, outputs, vocab } = this.createTrainingData(rawData);

        const model = this.createModel(vocab.total_tokens);

        console.log('Training model...');
        const history = await model.fit(inputs, outputs, {
            epochs: 50, // Reduced epochs to prevent potential memory issues
            batchSize: 32, // Reduced batch size
            validationSplit: 0.2,
            callbacks: {
                onEpochEnd: async (epoch, logs) => {
                    console.log(`Epoch ${epoch}: loss = ${logs.loss}, accuracy = ${logs.accuracy}`);
                }
            }
        });

        // Save model, vocab, and weights
        const modelPath = './ai_model';
        await model.save(`file://${modelPath}`);
        
        fs.writeFileSync(
            path.join(modelPath, 'vocab.json'),
            JSON.stringify(vocab, null, 2)
        );

        console.log('Model training complete.');
    }
}

// Run training
const trainer = new ArduinoCodeTrainer('./arduino_code');
trainer.train().catch(console.error);
