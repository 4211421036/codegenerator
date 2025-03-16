async function generateCode() {
    const input = document.getElementById('input').value;
    const output = document.getElementById('generated-code');
    
    output.textContent = 'Generating code...';
    
    try {
        const repo = '4211421036/codegenerator'; // Ganti dengan repo Anda
        const token = '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'; // Ganti dengan PAT Anda
        
        const response = await fetch(
            `https://api.github.com/repos/${repo}/dispatches`,
            {
                method: 'POST',
                headers: {
                    'Authorization': `token ${token}`,
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
