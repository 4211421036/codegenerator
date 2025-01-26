import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

class TemplateExtractor {
    constructor(folderPath, maxFiles = 500, maxLength = 500) {
        this.folderPath = folderPath;
        this.maxFiles = maxFiles;
        this.maxLength = maxLength;
    }

    extractTemplates() {
        const files = fs.readdirSync(this.folderPath)
            .filter(file => file.endsWith('.ino'))
            .slice(0, this.maxFiles);

        const templates = [];
        const vocab = new Set();

        files.forEach(file => {
            const content = fs.readFileSync(path.join(this.folderPath, file), 'utf8');
            const fileTemplate = this.analyzeFile(content);

            templates.push(fileTemplate);
            this.updateVocabulary(vocab, fileTemplate.keywords);
        });

        this.saveTemplates(templates, vocab);
    }

    analyzeFile(content) {
        return {
            pinConfigurations: this.extractPinConfigurations(content),
            functions: this.extractFunctions(content),
            keywords: this.extractKeywords(content),
            libraries: this.extractLibraries(content),
            sensors: this.detectSensors(content),
            globalVariables: this.extractGlobalVariables(content),
            controlStructures: this.analyzeControlStructures(content),
        };
    }

    extractPinConfigurations(content) {
        const pinConfigs = [];
        const pinRegex = /pinMode\((\d+|\w+),\s*(OUTPUT|INPUT|INPUT_PULLUP)\)/g;
        let match;

        while ((match = pinRegex.exec(content)) !== null) {
            pinConfigs.push({ pin: match[1], mode: match[2] });
        }

        return pinConfigs;
    }

    extractFunctions(content) {
        const functions = [];
        const functionRegex = /void\s+(\w+)\((.*?)\)\s*{([\s\S]*?)}/g;
        let match;

        while ((match = functionRegex.exec(content)) !== null) {
            functions.push({
                name: match[1],
                parameters: match[2].trim(),
                body: match[3].trim()
            });
        }

        return functions;
    }

    extractKeywords(content) {
        const keywordRegex = /\b(digitalWrite|analogRead|analogWrite|delay|Serial|attachInterrupt|EEPROM|Wire)\b/g;
        const keywords = new Set();
        let match;

        while ((match = keywordRegex.exec(content)) !== null) {
            keywords.add(match[1]);
        }

        return Array.from(keywords);
    }

    extractLibraries(content) {
        const libraries = [];
        const libraryRegex = /#include\s+<([^>]+)>/g;
        let match;

        while ((match = libraryRegex.exec(content)) !== null) {
            libraries.push(match[1]);
        }

        return libraries;
    }

    detectSensors(content) {
        const sensors = ["temperature", "humidity", "light", "distance", "motion", "pressure"];
        return sensors.filter(sensor => content.toLowerCase().includes(sensor));
    }

    extractGlobalVariables(content) {
        const globalVars = [];
        const globalVarRegex = /^(int|float|char|String|bool)\s+(\w+)\s*=\s*([^;]+);/gm;
        let match;

        while ((match = globalVarRegex.exec(content)) !== null) {
            globalVars.push({ type: match[1], name: match[2], value: match[3] });
        }

        return globalVars;
    }

    analyzeControlStructures(content) {
        const structures = [];
        const controlRegex = /\b(if|for|while|switch|case)\b/g;
        let match;

        while ((match = controlRegex.exec(content)) !== null) {
            structures.push(match[1]);
        }

        return structures;
    }

    updateVocabulary(vocab, keywords) {
        keywords.forEach(keyword => vocab.add(keyword));
    }

    // Add async here
    async saveTemplates(templates, vocab) {
        const modelPath = './ai_model';
        
        // Create and train a model (simple example)
        const model = tf.sequential();
        model.add(tf.layers.dense({ units: 10, inputShape: [10] }));
        model.add(tf.layers.dense({ units: 1, activation: 'sigmoid' }));

        model.compile({ optimizer: 'adam', loss: 'binaryCrossentropy', metrics: ['accuracy'] });

        // Dummy data for training (you will need to modify this for your case)
        const xs = tf.tensor2d([[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]]);
        const ys = tf.tensor2d([[1]]);

        await model.fit(xs, ys, { epochs: 10 });

        // Save the trained model
        await model.save(`file://${modelPath}`);

        const vocabPath = path.join(modelPath, 'vocab.json');
        // Dynamically save the extracted templates to model.json
        const modelJson = {
            templates: templates,
            vocab: Array.from(vocab)
        };

        // Save both model and vocabulary
        fs.writeFileSync(path.join(modelPath, 'model.json'), JSON.stringify(modelJson, null, 2));
        fs.writeFileSync(vocabPath, JSON.stringify(Array.from(vocab), null, 2));

        console.log('Templates and vocabulary saved.');
    }
}

// Run the extraction
const extractor = new TemplateExtractor('./arduino_code');
extractor.extractTemplates();
