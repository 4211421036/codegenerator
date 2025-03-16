const express = require('express');
const { exec } = require('child_process');
const app = express();
const port = 3000;

app.use(express.json());

app.post('/generate', (req, res) => {
    const description = req.body.description;
    exec(`python3 generate/generate_code.py "${description}"`, (error, stdout, stderr) => {
        if (error) {
            console.error(`Error: ${stderr}`);
            return res.status(500).json({ error: 'Failed to generate code' });
        }
        const code = require('fs').readFileSync('website/generated_code.ino', 'utf-8');
        res.json({ code });
    });
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});
