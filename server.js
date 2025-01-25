const express = require('express');
const fetch = require('node-fetch');
const app = express();
const port = 3000;

const GH_TOKEN = process.env.GH_TOKEN;  // Gunakan token GitHub yang aman

// Endpoint untuk mencari kode dari GitHub
app.get('/search/github', async (req, res) => {
    const query = req.query.query;
    const url = `https://api.github.com/search/code?q=${encodeURIComponent(query)}+in:file+language:ino`;

    try {
        const response = await fetch(url, {
            method: 'GET',
            headers: {
                'Authorization': `Bearer ${GH_TOKEN}`,
                'Accept': 'application/vnd.github.v3+json'
            }
        });
        const data = await response.json();
        if (response.ok) {
            res.json(data.items);  // Kirimkan data ke frontend
        } else {
            res.status(500).json({ error: 'GitHub API Error', message: data.message });
        }
    } catch (error) {
        res.status(500).json({ error: 'Request Failed', message: error.message });
    }
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});
