import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

class ArduinoCodeTrainer {
    constructor(folderPath) {
        this.folderPath = folderPath;
        this.maxLength = 100; // Further reduced
        this.maxFiles = 200;  // Significantly reduced
    }

    preprocessData() {
        const files = fs.readdirSync(this.folderPath)
            .filter(file => file.endsWith('.ino'))
            .slice(0, this.maxFiles);

        return files.map(file => {
            const content = fs.readFileSync(path.join(this.folderPath, file), 'utf8');
            return {
                filename: file,
                input: file.replace('.ino', ''),
                output: content
            };
        });
    }

    tokenize(text) {
        return text
            .toLowerCase()
            .replace(/[^\w\s.;(){}[\]=+\-*/&|<>!#]/g, ' ')
            .split(/\s+/)
            .filter(token => token.length > 0)
            .slice(0, this.maxLength);
    }

    createTrainingData(data) {
        const allTokens = new Set(['<PAD>', '<START>', '<END>']);
        
        // Collect unique tokens first
        data.forEach(({ input, output }) => {
            this.tokenize(input).forEach(token => allTokens.add(token));
            this.tokenize(output).forEach(token => allTokens.add(token));
        });

        const tokenToIndex = new Map(
            Array.from(allTokens).map((token, index) => [token, index])
        );

        const inputs = [];
        const outputs = [];

        data.forEach(({ input, output }) => {
            const inputTokens = ['<START>', ...this.tokenize(input), '<END>'];
            const outputTokens = ['<START>', ...this.tokenize(output), '<END>'];

            const paddedInput = [
                ...inputTokens.slice(0, this.maxLength),
                ...Array(Math.max(0, this.maxLength - inputTokens.length)).fill('<PAD>')
            ];

            const paddedOutput = [
                ...outputTokens.slice(0, this.maxLength),
                ...Array(Math.max(0, this.maxLength - outputTokens.length)).fill('<PAD>')
            ];

            inputs.push(paddedInput.map(token => tokenToIndex.get(token) || 0));
            outputs.push(paddedOutput.map(token => {
                const oneHot = new Array(allTokens.size).fill(0);
                const index = tokenToIndex.get(token);
                if (index !== undefined) oneHot[index] = 1;
                return oneHot;
            }));
        });

        return {
            inputs: tf.tensor2d(inputs, [inputs.length, this.maxLength]),
            outputs: tf.tensor3d(outputs, [outputs.length, this.maxLength, allTokens.size]),
            vocab: {
                vocabulary: Array.from(allTokens),
                total_tokens: allTokens.size,
                tokenToIndex: Object.fromEntries(tokenToIndex)
            }
        };
    }

    createModel(vocabSize) {
        const model = tf.sequential();
    
        model.add(tf.layers.embedding({
            inputDim: vocabSize,
            outputDim: 64, // Further reduced
            inputLength: this.maxLength
        }));
    
        model.add(tf.layers.globalAveragePooling1d()); // Replace LSTM with simpler layer
    
        model.add(tf.layers.dense({
            units: 128,
            activation: 'relu'
        }));
    
        model.add(tf.layers.dense({
            units: vocabSize,
            activation: 'softmax'
        }));
    
        model.compile({
            loss: 'categoricalCrossentropy',
            optimizer: 'adam',
            metrics: ['accuracy']
        });
    
        return model;
    }

    async train() {
        // Explicitly free memory before training
        global.gc && global.gc();

        const rawData = this.preprocessData();
        const { inputs, outputs, vocab } = this.createTrainingData(rawData);

        const model = this.createModel(vocab.total_tokens);

        const splitIndex = Math.floor(inputs.shape[0] * 0.8);
        const xTrain = inputs.slice([0, 0], [splitIndex, this.maxLength]);
        const yTrain = outputs.slice([0, 0, 0], [splitIndex, this.maxLength, vocab.total_tokens]);
        const xVal = inputs.slice([splitIndex, 0], [-1, this.maxLength]);
        const yVal = outputs.slice([splitIndex, 0, 0], [-1, this.maxLength, vocab.total_tokens]);

        const history = await model.fit(xTrain, yTrain, {
            epochs: 20, // Significantly reduced
            batchSize: 8, // Very small batch size
            validationData: [xVal, yVal],
            verbose: 1
        });

        const modelPath = './ai_model';
        await model.save(`file://${modelPath}`);

        // Write minimal model details
        fs.writeFileSync(
            path.join(modelPath, 'vocab.json'),
            JSON.stringify({
                vocabulary: vocab.vocabulary.slice(0, 1000), // Limit vocabulary size
                total_tokens: vocab.total_tokens,
                training_metrics: {
                    best_accuracy: Math.max(...(history.history.accuracy || [0])),
                    final_loss: history.history.loss[history.history.loss.length - 1]
                }
            }, null, 2)
        );

        console.log('Model training complete.');
        return { trainingComplete: true };
    }
}

// Increase Node.js memory and enable garbage collection
process.env.NODE_OPTIONS = '--max_old_space_size=4096 --expose-gc';

const trainer = new ArduinoCodeTrainer('./arduino_code');
trainer.train().catch(console.error);
