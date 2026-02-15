// Options page script
const DEFAULT_SHEET_URL = 'https://docs.google.com/spreadsheets/d/1p7n7mjurMZWstJKEeQw7gMici6VOOdlScVB6Jav0HVM';

// Load saved settings
document.addEventListener('DOMContentLoaded', async () => {
  const result = await chrome.storage.sync.get(['sheetUrl']);
  document.getElementById('sheetUrl').value = result.sheetUrl || DEFAULT_SHEET_URL;
});

// Save settings
document.getElementById('save').addEventListener('click', async () => {
  const sheetUrl = document.getElementById('sheetUrl').value.trim();
  
  if (!sheetUrl) {
    showStatus('Please enter a Google Sheet URL', 'error');
    return;
  }

  // Validate URL format
  if (!sheetUrl.match(/https:\/\/docs\.google\.com\/spreadsheets\/d\/[a-zA-Z0-9-_]+/)) {
    showStatus('Invalid Google Sheet URL format', 'error');
    return;
  }

  await chrome.storage.sync.set({ sheetUrl });
  showStatus('Settings saved successfully!', 'success');
});

// Test connection
document.getElementById('test').addEventListener('click', async () => {
  const sheetUrl = document.getElementById('sheetUrl').value.trim();
  
  if (!sheetUrl) {
    showStatus('Please enter a Google Sheet URL first', 'error');
    return;
  }

  try {
    showStatus('Testing connection...', 'success');
    
    // Extract spreadsheet ID
    const match = sheetUrl.match(/\/spreadsheets\/d\/([a-zA-Z0-9-_]+)/);
    if (!match) {
      showStatus('Invalid Google Sheet URL', 'error');
      return;
    }
    
    const spreadsheetId = match[1];
    
    // Get auth token
    const token = await new Promise((resolve, reject) => {
      chrome.identity.getAuthToken({ interactive: true }, (token) => {
        if (chrome.runtime.lastError) {
          reject(chrome.runtime.lastError);
        } else {
          resolve(token);
        }
      });
    });

    // Try to read the sheet
    const response = await fetch(
      `https://sheets.googleapis.com/v4/spreadsheets/${spreadsheetId}`,
      {
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        }
      }
    );

    if (response.ok) {
      const data = await response.json();
      showStatus(`✓ Connected successfully to: ${data.properties.title}`, 'success');
    } else {
      const error = await response.text();
      showStatus(`Connection failed: ${response.statusText}`, 'error');
      console.error('Test error:', error);
    }
  } catch (error) {
    showStatus(`Error: ${error.message}`, 'error');
    console.error('Test error:', error);
  }
});

// Show status message
function showStatus(message, type) {
  const statusDiv = document.getElementById('status');
  statusDiv.textContent = message;
  statusDiv.className = `status ${type}`;
  statusDiv.style.display = 'block';
  
  // Auto-hide after 5 seconds
  setTimeout(() => {
    statusDiv.style.display = 'none';
  }, 5000);
}

