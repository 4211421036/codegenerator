import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

class ArduinoCodeTrainer {
    constructor(folderPath) {
        this.folderPath = folderPath;
        this.maxLength = 150; // Reduced from 200
        this.maxFiles = 500;  // Reduced from 1000
    }

    preprocessData() {
        const files = fs.readdirSync(this.folderPath);
        const data = [];

        for (const file of files.slice(0, this.maxFiles)) {
            if (file.endsWith('.ino')) {
                const content = fs.readFileSync(path.join(this.folderPath, file), 'utf8');
                data.push({
                    filename: file,
                    input: file.replace('.ino', ''),
                    output: content
                });
            }
        }
        return data;
    }

    tokenize(text) {
        return text
            .toLowerCase()
            .replace(/[^\w\s.;(){}[\]=+\-*/&|<>!#]/g, ' ')
            .split(/\s+/)
            .filter(token => token.length > 0)
            .slice(0, this.maxLength);
    }

    extractAdvancedFeatures(data) {
        // [Previous implementation remains the same]
        // ... (keep the existing extractAdvancedFeatures method)
    }

    createTrainingData(data) {
        const allTokens = new Set(['<PAD>', '<START>', '<END>']);
    
        data.forEach(({ input, output }) => {
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
    
            const oneHotOutput = paddedOutput.map(token => {
                const oneHot = new Array(allTokens.size).fill(0);
                const index = tokenToIndex.get(token);
                if (index !== undefined) {
                    oneHot[index] = 1;
                }
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
            outputDim: 128, // Reduced from 256
            inputLength: this.maxLength
        }));
    
        model.add(tf.layers.lstm({
            units: 256, // Reduced from 512
            returnSequences: true
        }));
    
        model.add(tf.layers.dropout({ rate: 0.2 })); // Reduced from 0.3
    
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
        const rawData = this.preprocessData();
        const { inputs, outputs, vocab } = this.createTrainingData(rawData);

        const model = this.createModel(vocab.total_tokens);
        const splitIndex = Math.floor(inputs.shape[0] * 0.8);
        const xTrain = inputs.slice([0, 0], [splitIndex, this.maxLength]);
        const yTrain = outputs.slice([0, 0, 0], [splitIndex, this.maxLength, vocab.total_tokens]);
        const xVal = inputs.slice([splitIndex, 0], [-1, this.maxLength]);
        const yVal = outputs.slice([splitIndex, 0, 0], [-1, this.maxLength, vocab.total_tokens]);

        const history = await model.fit(xTrain, yTrain, {
            epochs: 50, // Reduced from 100
            batchSize: 16, // Reduced from 32
            validationData: [xVal, yVal],
            verbose: 1,
            callbacks: {
                onEpochEnd: async (epoch, logs) => {
                    console.log(`Epoch ${epoch + 1}:`, 
                        `loss=${logs.loss.toFixed(4)}, ` +
                        `accuracy=${(logs.accuracy * 100).toFixed(2)}%, ` +
                        `val_loss=${logs.val_loss.toFixed(4)}, ` +
                        `val_accuracy=${(logs.val_accuracy * 100).toFixed(2)}%`
                    );
                }
            }
        });

        const modelPath = './ai_model';
        await model.save(`file://${modelPath}`);
        
        const modelDetails = this.extractAdvancedFeatures(rawData);
        modelDetails.trainingMetrics = {
            accuracyHistory: history.history.accuracy || [],
            valAccuracyHistory: history.history.val_accuracy || [],
            lossHistory: history.history.loss || [],
            valLossHistory: history.history.val_loss || [],
            bestAccuracy: Math.max(...(history.history.accuracy || [0])),
            bestValAccuracy: Math.max(...(history.history.val_accuracy || [0])),
            finalLoss: history.history.loss ? history.history.loss[history.history.loss.length - 1] : null,
            finalValLoss: history.history.val_loss ? history.history.val_loss[history.history.val_loss.length - 1] : null
        };
        
        fs.writeFileSync(
            path.join(modelPath, 'model.json'),
            JSON.stringify(modelDetails, null, 2)
        );

        fs.writeFileSync(
            path.join(modelPath, 'vocab.json'),
            JSON.stringify(vocab, null, 2)
        );

        console.log('Model training complete.');
        return modelDetails;
    }
}

// Increase Node.js memory limit
process.env.NODE_OPTIONS = '--max_old_space_size=4096';

const trainer = new ArduinoCodeTrainer('./arduino_code');
trainer.train().catch(console.error);
