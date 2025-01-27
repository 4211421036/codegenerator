import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

class TemplateExtractor {
    constructor(folderPath, maxFiles = 4000, maxLength = 500) {
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
            const template = this.analyzeFile(content);
            templates.push(template);

            template.keywords.forEach(keyword => vocab.add(keyword));
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

    extractTemplateName(content) {
        const match = /#define\s+PROJECT_NAME\s+["'](.+?)["']/.exec(content);
        return match ? match[1] : 'Unnamed Template';
    }

    extractKeywords(content) {
        const keywordPatterns = {
            // IoT Platforms & Services
            iotPlatforms: /\b(Blynk|Firebase|ThingSpeak|IFTTT|Telegram|AdafruitIO|AWSIoT|AzureIoT|GoogleCloud|Mosquitto|NodeRed)\b/gi,

            // IoT Communication
            iotCommunication: /\b(MQTT|HTTP|WebSocket|REST|API|WebServer|ESP8266WebServer|AsyncWebServer|WiFiClient|WiFiServer|PubSubClient)\b/gi,

            // IoT Authentication & Security
            iotAuth: /\b(token|auth|key|password|encryption|SSL|TLS|certificate|HTTPS|SHA256|MD5)\b/gi,

            // IoT Data
            iotData: /\b(JSON|payload|publish|subscribe|callback|stream|buffer|parse|stringify|database)\b/gi,

            // Sensors
            sensors: /\b(DHT11|DHT22|BMP280|BME280|MPU6050|HC-SR04|DS18B20|LDR|PIR|ADXL345|AHT10|SHT31|TSL2561|BH1750|INA219)\b/gi,

            // Physical parameters
            parameters: /\b(temperature|humidity|pressure|acceleration|distance|light|motion|pH|soil|moisture|voltage|current|power|energy|airquality|CO2|particulate|PM2\.5|PM10)\b/gi,

            // Communication protocols
            protocols: /\b(I2C|SPI|UART|OneWire|WiFi|Bluetooth|BLE|LoRa|LoRaWAN|NB-IoT|GSM|GPRS|4G|LTE|5G|NTP|DNS)\b/gi,

            // Common Arduino functions
            functions: /\b(digitalWrite|analogRead|analogWrite|digitalRead|pulseIn|tone|servo|millis|delay|yield|ESP\.restart|ESP\.deepSleep)\b/g,

            // Display and output
            display: /\b(LCD|OLED|TFT|LED|RGB|segment|matrix|display|Nextion|ST7789|ILI9341|SSD1306)\b/gi,

            // Storage and memory
            storage: /\b(EEPROM|SD|Flash|SPIFFS|LittleFS|NVS|preferences)\b/gi,

            // Timing and control
            timing: /\b(timer|interrupt|PWM|PID|debounce|watchdog|schedule|cron|RTC|timestamp)\b/gi,

            // Web Interface
            webInterface: /\b(HTML|CSS|JavaScript|Ajax|fetch|XMLHttpRequest|Bootstrap|jQuery|WebSocket|SSE|EventSource)\b/gi
        };

        const keywords = new Set();
        
        // Extract keywords from each category
        for (const [category, pattern] of Object.entries(keywordPatterns)) {
            let match;
            while ((match = pattern.exec(content)) !== null) {
                keywords.add(match[0].toLowerCase());
            }
        }

        return Array.from(keywords);
    }

    extractLibraries(content) {
        const libraries = new Set();
        const libraryRegex = /#include\s+[<"]([^>"]+)[>"]/g;
        let match;

        while ((match = libraryRegex.exec(content)) !== null) {
            libraries.add(match[1]);
        }

        return Array.from(libraries);
    }

    extractFunctions(content) {
        const functions = [];
        const functionRegex = /(void|int|bool|String|float|double)\s+(\w+)\s*\((.*?)\)\s*{([\s\S]*?)}/g;
        let match;

        while ((match = functionRegex.exec(content)) !== null) {
            functions.push({
                name: match[2],
                parameters: match[3],
                body: match[4],
            });
        }

        return functions;
    }

    saveTemplates(templates, vocab) {
        const outputPath = './ai_model/model.json';
        const modelData = {
            templates,
            vocab: Array.from(vocab),
        };

        fs.writeFileSync(outputPath, JSON.stringify(modelData, null, 2));
        console.log(`Templates saved to ${outputPath}`);
    }
}

// Logika Gabungan untuk Membuat Kode
function combineTemplates(description, templates) {
    const detectedKeywords = description.toLowerCase().split(/\s+/);
    const matchedTemplates = templates.filter(template =>
        template.keywords.some(keyword => detectedKeywords.includes(keyword))
    );

    const libraries = new Set();
    let setupCode = '';
    let loopCode = '';

    matchedTemplates.forEach(template => {
        template.libraries.forEach(lib => libraries.add(lib));
        template.functions.forEach(func => {
            if (func.name === 'setup') setupCode += func.body + '\n';
            if (func.name === 'loop') loopCode += func.body + '\n';
        });
    });

    // Final generated code
    const generatedCode = `
        ${Array.from(libraries).map(lib => `#include <${lib}>`).join('\n')}

        void setup() {
            ${setupCode}
        }

        void loop() {
            ${loopCode}
        }
    `;

    return generatedCode.trim();
}

// Ekstraksi Template dan Penggunaan
const extractor = new TemplateExtractor('./arduino_code');
extractor.extractTemplates();

// Gunakan data yang diekstrak
const modelPath = './ai_model/model.json';
const modelData = JSON.parse(fs.readFileSync(modelPath, 'utf8'));
const description = 'Buatkan saya code Arduino DHT11 terintegrasi Firebase WebServer.';
const generatedCode = combineTemplates(description, modelData.templates);

console.log(generatedCode);
