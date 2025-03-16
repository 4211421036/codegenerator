document.getElementById('codeForm').addEventListener('submit', async (e) => {
    e.preventDefault();
    
    const input = document.getElementById('input').value;
    const output = document.getElementById('output');
    
    output.textContent = 'Generating code...';
    
    try {
        // Save input to a file (using GitHub Pages)
        const response = await fetch('/inputs/user_input.txt', {
            method: 'POST',
            headers: {
                'Content-Type': 'text/plain'
            },
            body: input
        });
        
        if (response.ok) {
            output.textContent = 'Code generation started! Check back in 5 minutes.';
            checkForOutput();  // Start checking for generated code
        } else {
            output.textContent = 'Error saving input. Please try again.';
        }
    } catch (error) {
        output.textContent = `Connection error: ${error.message}`;
    }
});

async function checkForOutput() {
    const output = document.getElementById('output');
    
    try {
        const response = await fetch('/outputs/generated_code.ino');
        if (response.ok) {
            const code = await response.text();
            output.textContent = code;
        } else {
            setTimeout(checkForOutput, 5000);  // Check again in 5 seconds
        }
    } catch (error) {
        output.textContent = `Error checking output: ${error.message}`;
    }
}
