import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

class ArduinoCodeTrainer {
    constructor(folderPath) {
        this.folderPath = folderPath;
        this.maxLength = 100;
        this.maxFiles = 50;
    }

    preprocessData() {
        const files = fs.readdirSync(this.folderPath);
        const data = [];

        for (const file of files.slice(0, this.maxFiles)) {
            if (file.endsWith('.ino')) {
                const content = fs.readFileSync(path.join(this.folderPath, file), 'utf8');
                data.push({
                    input: file.replace('.ino', ''),
                    output: content
                });
            }
        }
        return data;
    }

    tokenize(text) {
        // More comprehensive tokenization
        return text
            .toLowerCase()
            .replace(/[^\w\s.;(){}[\]=+\-*/&|<>!]/g, ' ')
            .split(/\s+/)
            .filter(token => token.length > 0)
            .slice(0, this.maxLength);
    }

    createTrainingData(data) {
        const allTokens = new Set(['<PAD>', '<START>', '<END>']);
    
        // Collect unique tokens
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
                ...inputTokens.slice(0, this.maxLength), // Truncate if exceeds maxLength
                ...Array(Math.max(0, this.maxLength - inputTokens.length)).fill('<PAD>') // Ensure valid padding length
            ];
    
            const paddedOutput = [
                ...outputTokens.slice(0, this.maxLength), // Truncate if exceeds maxLength
                ...Array(Math.max(0, this.maxLength - outputTokens.length)).fill('<PAD>') // Ensure valid padding length
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
            outputDim: 128,
            inputLength: this.maxLength
        }));

        model.add(tf.layers.lstm({
            units: 256,
            returnSequences: true
        }));

        model.add(tf.layers.dropout({ rate: 0.2 }));

        model.add(tf.layers.lstm({
            units: 256
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
        const rawData = this.preprocessData();
        const { inputs, outputs, vocab } = this.createTrainingData(rawData);

        const model = this.createModel(vocab.total_tokens);

        await model.fit(inputs, outputs, {
            epochs: 50,
            batchSize: 32,
            verbose: 1
        });

        const modelPath = './ai_model';
        await model.save(`file://${modelPath}`);
        
        fs.writeFileSync(
            path.join(modelPath, 'vocab.json'),
            JSON.stringify(vocab, null, 2)
        );

        console.log('Model training complete.');
    }
}

const trainer = new ArduinoCodeTrainer('./arduino_code');
trainer.train().catch(console.error);
