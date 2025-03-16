async function generateCode() {
    const input = document.getElementById('input').value;
    const output = document.getElementById('generated-code');
    
    output.textContent = 'Generating code...';
    
    try {
        const response = await fetch(
            'https://api.github.com/repos/4211421036/codegenerator/dispatches',
            {
                method: 'POST',
                headers: {
                    'Accept': 'application/vnd.github.everest-preview+json',
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    event_type: 'generate_code',
                    client_payload: { input }
                })
            }
        );
        
        if (response.ok) {
            output.textContent = 'Code generation started! Check back in 5 minutes.';
        } else {
            const errorData = await response.json();
            output.textContent = `Error: ${errorData.message || 'Unknown error'}`;
            console.error('API Error:', errorData);
        }
    } catch (error) {
        output.textContent = `Connection error: ${error.message}`;
        console.error('Network Error:', error);
    }
}

async function checkGeneratedCode() {
    try {
        const response = await fetch('outputs/generated_code.ino');
        if (response.ok) {
            const code = await response.text();
            document.getElementById('generated-code').textContent = code;
        } else {
            document.getElementById('generated-code').textContent = 'No code generated yet.';
        }
    } catch (error) {
        console.error('Error fetching generated code:', error);
        document.getElementById('generated-code').textContent = 'Error loading generated code.';
    }
}

// Check for generated code every 10 seconds
setInterval(checkGeneratedCode, 10000);
