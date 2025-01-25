import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

class ArduinoCodeTrainer {
    constructor(folderPath) {
        this.folderPath = folderPath;
        this.maxLength = 50; // Reduced max length
        this.maxFiles = 10; // Limit number of files
    }

    preprocessData() {
        try {
            const files = fs.readdirSync(this.folderPath);
            const data = [];

            for (const file of files.slice(0, this.maxFiles)) {
                if (file.endsWith('.ino')) {
                    try {
                        const content = fs.readFileSync(path.join(this.folderPath, file), 'utf8');
                        data.push({
                            input: file.replace('.ino', ''),
                            output: content.trim()
                        });
                    } catch (fileReadError) {
                        console.error(`Error reading file ${file}:`, fileReadError);
                    }
                }
            }
            return data;
        } catch (error) {
            console.error('Error in preprocessData:', error);
            return [];
        }
    }

    tokenize(text) {
        return text
            .toLowerCase()
            .replace(/[^\w\s]/g, '')
            .split(/\s+/)
            .filter(token => token.length > 0)
            .slice(0, this.maxLength);
    }

    createTrainingData(data) {
        if (data.length === 0) {
            throw new Error('No data to process');
        }

        const allTokens = new Set(['<PAD>', '<START>', '<END>']);

        // Collect unique tokens
        for (const { input, output } of data) {
            const inputTokens = this.tokenize(input);
            const outputTokens = this.tokenize(output);

            inputTokens.forEach(token => allTokens.add(token));
            outputTokens.forEach(token => allTokens.add(token));
        }

        const tokenToIndex = new Map(
            Array.from(allTokens).map((token, index) => [token, index])
        );

        const inputs = [];
        const outputs = [];

        for (const { input, output } of data) {
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
        }

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
            outputDim: 64, // Reduced embedding size
            inputLength: this.maxLength
        }));

        model.add(tf.layers.lstm({
            units: 128, // Reduced units
            returnSequences: false // Changed to false
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
        try {
            const rawData = this.preprocessData();
            console.log(`Processing ${rawData.length} files`);

            const { inputs, outputs, vocab } = this.createTrainingData(rawData);

            const model = this.createModel(vocab.total_tokens);

            console.log('Training model...');
            await model.fit(inputs, outputs, {
                epochs: 10,
                batchSize: 16,
                verbose: 1
            });

            const modelPath = './ai_model';
            await model.save(`file://${modelPath}`);
            
            fs.writeFileSync(
                path.join(modelPath, 'vocab.json'),
                JSON.stringify(vocab, null, 2)
            );

            console.log('Model training complete.');
        } catch (error) {
            console.error('Training failed:', error);
        }
    }
}

// Run training
const trainer = new ArduinoCodeTrainer('./arduino_code');
trainer.train().catch(console.error);
