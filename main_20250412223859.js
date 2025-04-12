
const axios = require('axios');

// Replace these placeholders with actual values
const API_KEY = '<replace_api_key>';
const EXTERNAL_USER_ID = '<replace_external_user_id>';
const QUERY = 'Put your query here';
const PLUGIN_IDS = [
  'plugin-1712327325',
  'plugin-1713962163',
  'plugin-1722260873',
  'plugin-1713954536',
  'plugin-1713958591',
  'plugin-1713958830',
  'plugin-1713961903',
  'plugin-1713967141'
];

// Create Chat Session
async function createChatSession() {
  try {
    const response = await axios.post(
      'https://api.on-demand.io/chat/v1/sessions',
      {
        pluginIds: [],
        externalUserId: EXTERNAL_USER_ID
      },
      {
        headers: {
          apikey: API_KEY
        }
      }
    );

    if (response.status === 201) {
      console.log('Chat session created successfully:', response.data);
      return response.data.id; // Extract session ID
    } else {
      throw new Error('Unexpected status code: ' + response.status);
    }
  } catch (error) {
    console.error('Error creating chat session:', error.message);
    throw error;
  }
}

// Submit Query (Sync Mode)
async function submitQuerySync(sessionId) {
  try {
    const response = await axios.post(
      `https://api.on-demand.io/chat/v1/sessions/${sessionId}/query`,
      {
        endpointId: 'predefined-openai-gpt4o',
        query: QUERY,
        pluginIds: PLUGIN_IDS,
        responseMode: 'sync',
        reasoningMode: 'medium'
      },
      {
        headers: {
          apikey: API_KEY
        }
      }
    );

    if (response.status === 200) {
      console.log('Query response:', response.data);
    } else {
      throw new Error('Unexpected status code: ' + response.status);
    }
  } catch (error) {
    console.error('Error submitting query:', error.message);
    throw error;
  }
}

// Submit Query (Stream Mode via SSE)
async function submitQueryStream(sessionId) {
  try {
    const url = `https://api.on-demand.io/chat/v1/sessions/${sessionId}/query`;
    const body = {
      endpointId: 'predefined-openai-gpt4o',
      query: QUERY,
      pluginIds: PLUGIN_IDS,
      responseMode: 'stream',
      reasoningMode: 'medium'
    };

    const headers = {
      apikey: API_KEY,
      'Content-Type': 'application/json'
    };

    const response = await axios.post(url, body, {
      headers,
      responseType: 'stream'
    });

    response.data.on('data', (chunk) => {
      const data = chunk.toString();
      console.log('Streamed data:', data);
    });

    response.data.on('end', () => {
      console.log('Stream ended.');
    });

    response.data.on('error', (error) => {
      console.error('Stream error:', error.message);
    });
  } catch (error) {
    console.error('Error submitting query in stream mode:', error.message);
    throw error;
  }
}

// Main function to execute the flow
async function main() {
  try {
    const sessionId = await createChatSession();
    console.log('Session ID:', sessionId);

    // Submit query in sync mode
    await submitQuerySync(sessionId);

    // Uncomment the following line to test stream mode
    // await submitQueryStream(sessionId);
  } catch (error) {
    console.error('Error in main flow:', error.message);
  }
}

// Execute the main function
main();
