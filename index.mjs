import * as tf from '@tensorflow/tfjs-node';
import fs from 'fs';
import path from 'path';

class TemplateExtractor {
    constructor(folderPath, maxFiles = 1000, maxLength = 500) {
        this.folderPath = folderPath;
        this.maxFiles = maxFiles;
        this.maxLength = maxLength;
        this.modelMetrics = {
            totalTemplates: 0,
            processedFiles: 0,
            uniqueKeywords: 0
        };
    }

    extractTemplates() {
        const files = fs.readdirSync(this.folderPath)
            .filter(file => file.endsWith('.ino'))
            .slice(0, this.maxFiles);

        const templates = [];
        const vocab = new Set();
        const keywordFrequency = new Map();

        files.forEach(file => {
            const content = fs.readFileSync(path.join(this.folderPath, file), 'utf8');
            const fileTemplate = this.analyzeFile(content);
            templates.push(fileTemplate);
            this.updateVocabulary(vocab, fileTemplate.keywords, keywordFrequency);
            this.modelMetrics.processedFiles++;
        });

        this.modelMetrics.totalTemplates = templates.length;
        this.modelMetrics.uniqueKeywords = vocab.size;

        this.saveTemplates(templates, vocab);
        this.saveMetrics();
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
        const projectNameRegex = /\/\*\s*Project:\s*([^\n\*]+)/i;
        const match = projectNameRegex.exec(content) || /#define\s+PROJECT_NAME\s+["']([^"']+)/.exec(content);
        return match ? match[1].trim() : 'Unnamed Template';
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

        // Extract variable names that might indicate IoT or sensor usage
        const variableRegex = /\b(?:sensor|probe|meter|detector|monitor|device|node|gateway|endpoint|broker|client)\w+/gi;
        let match;
        while ((match = variableRegex.exec(content)) !== null) {
            keywords.add(match[0].toLowerCase());
        }

        // Extract common IoT configuration patterns
        const configPatterns = [
            /define\s+(?:WIFI_SSID|WIFI_PASS|AUTH_TOKEN|API_KEY|SERVER|HOST|MQTT_BROKER|BOT_TOKEN)\s+["']([^"']+)/g,
            /\.connect\(.*["']([^"']+)["']/g,
            /\.begin\(.*["']([^"']+)["']/g
        ];

        configPatterns.forEach(pattern => {
            while ((match = pattern.exec(content)) !== null) {
                if (match[1].includes('blynk') || match[1].includes('mqtt') || match[1].includes('firebase')) {
                    keywords.add(match[1].toLowerCase());
                }
            }
        });

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
        const functionRegex = /(?:void|int|bool|String|float|double)\s+(\w+)\s*\((.*?)\)\s*{([\s\S]*?)\n}/g;
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

    updateVocabulary(vocab, keywords, keywordFrequency) {
        keywords.forEach(keyword => {
            vocab.add(keyword);
            keywordFrequency.set(keyword, (keywordFrequency.get(keyword) || 0) + 1);
        });
    }

    saveTemplates(templates, vocab) {
        const modelPath = './ai_model';
        if (!fs.existsSync(modelPath)) {
            fs.mkdirSync(modelPath, { recursive: true });
        }

        const modelJson = {
            templates: templates,
            vocab: Array.from(vocab)
        };

        fs.writeFileSync(
            path.join(modelPath, 'model.json'), 
            JSON.stringify(modelJson, null, 2)
        );
    }

    saveMetrics() {
        const metricsPath = './ai_model/training_metrics.json';
        fs.writeFileSync(metricsPath, JSON.stringify(this.modelMetrics, null, 2));
    }
}

// Run the extraction
const extractor = new TemplateExtractor('./arduino_code');
extractor.extractTemplates();
