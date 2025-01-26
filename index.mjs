import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

class TemplateExtractor {
    constructor(folderPath, maxFiles = 1000, maxLength = 500) {
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
            name: this.extractTemplateName(content),
            keywords: this.extractKeywords(content),
            libraries: this.extractLibraries(content),
            functions: this.extractFunctions(content),
        };
    }

    // Extract template name from the file content or filename
    extractTemplateName(content) {
        const match = /#define\s+(\w+)/.exec(content);
        return match ? match[1] : 'Unnamed Template';
    }

    extractFunctions(content) {
        const functions = [];
        const functionRegex = /void\s+(\w+)\((.*?)\)\s*{([\s\S]*?)}/g;
        let match;

        while ((match = functionRegex.exec(content)) !== null) {
            functions.push({
                name: match[1],
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

    updateVocabulary(vocab, keywords) {
        keywords.forEach(keyword => vocab.add(keyword));
    }

    // Save the templates and vocabulary as model.json
    async saveTemplates(templates, vocab) {
        const modelPath = './ai_model';

        const modelJson = {
            templates: templates,
            vocab: Array.from(vocab)
        };

        // Save both templates and vocabulary
        fs.writeFileSync(path.join(modelPath, 'model.json'), JSON.stringify(modelJson, null, 2));

        const vocabPath = path.join(modelPath, 'vocab.json');
        fs.writeFileSync(vocabPath, JSON.stringify(Array.from(vocab), null, 2));

        console.log('Templates and vocabulary saved.');
    }
}

// Run the extraction
const extractor = new TemplateExtractor('./arduino_code'); // Path to your Arduino code folder
extractor.extractTemplates();
