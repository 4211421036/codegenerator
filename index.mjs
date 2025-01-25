import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

class ArduinoCodeTrainer {
    constructor(folderPath) {
        this.folderPath = folderPath;
        this.maxLength = 200;
        this.maxFiles = 100;
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
        const model = {
            libraryIncludes: {},
            pinConfigurations: {},
            functionTemplates: {},
            serialConfigurations: {},
            commonKeywords: {},
            filePatterns: {},
            sensorTypes: {},
            communicationProtocols: {}
        };

        data.forEach(({ filename, output }) => {
            // Extract library includes
            const libraryMatches = output.match(/#include\s*<(\w+\.h)>/g);
            if (libraryMatches) {
                libraryMatches.forEach(lib => {
                    const libName = lib.match(/#include\s*<(\w+\.h)>/)[1];
                    model.libraryIncludes[libName] = (model.libraryIncludes[libName] || 0) + 1;
                });
            }

            // Extract pin configurations
            const pinMatches = output.match(/pinMode\((\d+),\s*(INPUT|OUTPUT|INPUT_PULLUP)\)/g);
            if (pinMatches) {
                pinMatches.forEach(match => {
                    const [, pin, mode] = match.match(/pinMode\((\d+),\s*(INPUT|OUTPUT|INPUT_PULLUP)\)/);
                    model.pinConfigurations[`pin_${pin}`] = { pin, mode };
                });
            }

            // Extract function structures
            const functionMatches = output.match(/void\s+(\w+)\(\)\s*{([^}]+)}/g);
            if (functionMatches) {
                functionMatches.forEach(match => {
                    const [, funcName, funcBody] = match.match(/void\s+(\w+)\(\)\s*{([^}]+)}/);
                    model.functionTemplates[funcName] = funcBody.trim();
                });
            }

            // Serial communication configuration
            const serialMatches = output.match(/Serial\.begin\((\d+)\)/);
            if (serialMatches) {
                const baudRate = serialMatches[1];
                model.serialConfigurations[baudRate] = (model.serialConfigurations[baudRate] || 0) + 1;
            }

            // Detect sensor and communication types
            const sensorKeywords = ['temperature', 'humidity', 'pressure', 'light', 'distance'];
            const communicationKeywords = ['WiFi', 'Bluetooth', 'I2C', 'SPI', 'MQTT'];

            sensorKeywords.forEach(sensor => {
                if (output.toLowerCase().includes(sensor)) {
                    model.sensorTypes[sensor] = (model.sensorTypes[sensor] || 0) + 1;
                }
            });

            communicationKeywords.forEach(protocol => {
                if (output.toLowerCase().includes(protocol.toLowerCase())) {
                    model.communicationProtocols[protocol] = (model.communicationProtocols[protocol] || 0) + 1;
                }
            });

            // Categorize file patterns
            const fileType = this.categorizeFile(output);
            model.filePatterns[fileType] = (model.filePatterns[fileType] || 0) + 1;

            // Count keyword frequencies
            const keywords = this.tokenize(output);
            keywords.forEach(keyword => {
                model.commonKeywords[keyword] = (model.commonKeywords[keyword] || 0) + 1;
            });
        });

        // Sort and limit results
        Object.keys(model).forEach(key => {
            if (typeof model[key] === 'object') {
                model[key] = Object.fromEntries(
                    Object.entries(model[key])
                        .sort((a, b) => b[1] - a[1])
                        .slice(0, 20)
                );
            }
        });

        return model;
    }

    categorizeFile(fileContent) {
        if (fileContent.includes('sensor')) return 'sensor';
        if (fileContent.includes('motor')) return 'motor';
        if (fileContent.includes('LED') || fileContent.includes('digitalWrite')) return 'led';
        if (fileContent.includes('WiFi') || fileContent.includes('network')) return 'network';
        return 'generic';
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
            outputDim: 256,
            inputLength: this.maxLength
        }));
    
        model.add(tf.layers.lstm({
            units: 512,
            returnSequences: true
        }));
    
        model.add(tf.layers.dropout({ rate: 0.3 }));
    
        model.add(tf.layers.lstm({
            units: 512,
            returnSequences: true
        }));
    
        model.add(tf.layers.dropout({ rate: 0.3 }));
    
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

        const history = await model.fit(inputs, outputs, {
            epochs: 100,
            batchSize: 32,
            validationSplit: 0.2,
            verbose: 1,
            callbacks: {
                onEpochEnd: async (epoch, logs) => {
                    console.log(`Epoch ${epoch + 1}:`, 
                        `loss=${logs.loss.toFixed(4)}, ` +
                        `accuracy=${logs.accuracy ? logs.accuracy.toFixed(4) : 'N/A'}`, 
                        `val_loss=${logs.val_loss.toFixed(4)}, ` +
                        `val_accuracy=${logs.val_accuracy ? logs.val_accuracy.toFixed(4) : 'N/A'}`
                    );
                }
            }
        });

        const modelPath = './ai_model';
        await model.save(`file://${modelPath}`);
        
        // Extract and save model details
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

const trainer = new ArduinoCodeTrainer('./arduino_code');
trainer.train().catch(console.error);
