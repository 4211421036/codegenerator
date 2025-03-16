async function generateCode() {
    const description = document.getElementById('description').value;
    const response = await fetch('/generate', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ description }),
    });
    const data = await response.json();
    document.getElementById('output').textContent = data.code;
}
