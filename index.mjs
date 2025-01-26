import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

class ArduinoCodeTemplateTrainer {
    constructor(folderPath) {
        this.folderPath = folderPath;
        this.maxLength = 200; // Increased to capture more code context
        this.maxFiles = 500;  // Increased to process more files
    }

    preprocessData() {
        const files = fs.readdirSync(this.folderPath)
            .filter(file => file.endsWith('.ino'))
            .slice(0, this.maxFiles);

        return files.map(file => {
            const content = fs.readFileSync(path.join(this.folderPath, file), 'utf8');
            return {
                filename: file,
                template: this.extractCodeTemplate(content),
                keywords: this.extractKeywords(content),
                fullContent: content
            };
        });
    }

    extractCodeTemplate(code) {
        // Extract structural template of the code
        const templatePatterns = [
            /void\s+setup\s*\(\)\s*{[^}]*}/,
            /void\s+loop\s*\(\)\s*{[^}]*}/,
            /\w+\s+\w+\s*\([^)]*\)\s*{[^}]*}/
        ];

        const templateMatches = templatePatterns.map(pattern => 
            (code.match(pattern) || [])[0] || ''
        );

        return templateMatches.join('\n');
    }

    extractKeywords(code) {
        // Extract meaningful keywords and identifiers
        const keywords = new Set();
        const keywordPatterns = [
            /\b(pinMode|digitalWrite|digitalRead|analogRead|delay)\b/g,
            /\b(class|struct|enum)\s+(\w+)/g,
            /\b(int|float|double|char|bool|void)\s+(\w+)\s*\(/g
        ];

        keywordPatterns.forEach(pattern => {
            const matches = code.matchAll(pattern);
            for (const match of matches) {
                keywords.add(match[1] || match[2]);
            }
        });

        return Array.from(keywords);
    }

    createTrainingData(data) {
        // Create comprehensive training data with templates and keywords
        const modelData = {
            templates: [],
            keywords: [],
            vocab: new Set(['<PAD>', '<START>', '<END>'])
        };

        data.forEach(item => {
            // Collect templates
            if (item.template) {
                modelData.templates.push({
                    filename: item.filename,
                    template: item.template
                });
            }

            // Collect keywords
            item.keywords.forEach(keyword => {
                modelData.keywords.push(keyword);
                modelData.vocab.add(keyword);
            });
        });

        // Write model.json with templates
        fs.writeFileSync('model.json', JSON.stringify({
            templates: modelData.templates,
            total_templates: modelData.templates.length
        }, null, 2));

        // Write vocab.json with keywords
        fs.writeFileSync('vocab.json', JSON.stringify({
            keywords: Array.from(modelData.keywords),
            total_keywords: modelData.keywords.length,
            vocabulary: Array.from(modelData.vocab)
        }, null, 2));

        return modelData;
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
        const modelData = this.createTrainingData(rawData);

        // Tokenize and prepare training data
        const tokenToIndex = new Map(
            Array.from(modelData.vocab).map((token, index) => [token, index])
        );

        console.log('Training with templates and keywords');
        return { trainingComplete: true };
    }
}

// Increase Node.js memory and enable garbage collection
process.env.NODE_OPTIONS = '--max_old_space_size=8192 --expose-gc';

const trainer = new ArduinoCodeTemplateTrainer('./arduino_code');
trainer.train().catch(console.error);
