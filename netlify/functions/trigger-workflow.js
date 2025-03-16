const fetch = require('node-fetch');

exports.handler = async (event) => {
    const { input } = JSON.parse(event.body);
    
    const response = await fetch(
        `https://api.github.com/repos/4211421036/codegenerator/dispatches`,
        {
            method: 'POST',
            headers: {
                'Authorization': `token ${process.env.GH_TOKEN}`,
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
        return {
            statusCode: 200,
            body: JSON.stringify({ message: 'Workflow triggered' })
        };
    } else {
        const errorData = await response.json();
        return {
            statusCode: response.status,
            body: JSON.stringify(errorData)
        };
    }
};
