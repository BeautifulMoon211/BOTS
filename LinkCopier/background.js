// LinkCopier - Supabase Version
// No OAuth required, no payment needed!

// Supabase Configuration
let SUPABASE_URL = '';
let SUPABASE_KEY = '';

// Load Supabase config from storage
async function loadSupabaseConfig() {
  const result = await chrome.storage.sync.get(['supabaseUrl', 'supabaseKey']);
  SUPABASE_URL = result.supabaseUrl || '';
  SUPABASE_KEY = result.supabaseKey || '';
  return { url: SUPABASE_URL, key: SUPABASE_KEY };
}

// Check if Supabase is configured
async function isConfigured() {
  const config = await loadSupabaseConfig();
  return config.url && config.key;
}

// Get the next row number
async function getNextRowNumber() {
  try {
    const config = await loadSupabaseConfig();
    
    // Get the highest number from the database
    const response = await fetch(
      `${config.url}/rest/v1/urls?select=number&order=number.desc&limit=1`,
      {
        headers: {
          'apikey': config.key,
          'Authorization': `Bearer ${config.key}`,
          'Content-Type': 'application/json'
        }
      }
    );

    if (!response.ok) {
      throw new Error(`Failed to read database: ${response.statusText}`);
    }

    const data = await response.json();
    
    if (data.length === 0) {
      return 1; // First entry
    }
    
    return (data[0].number || 0) + 1;
  } catch (error) {
    console.error('Error getting next number:', error);
    return 1;
  }
}

// Add URL to Supabase database
async function addUrlToDatabase(url) {
  try {
    const configured = await isConfigured();
    if (!configured) {
      throw new Error('Supabase not configured. Please set up in Settings.');
    }

    const config = await loadSupabaseConfig();
    const nextNumber = await getNextRowNumber();

    // Insert the new URL
    const response = await fetch(
      `${config.url}/rest/v1/urls`,
      {
        method: 'POST',
        headers: {
          'apikey': config.key,
          'Authorization': `Bearer ${config.key}`,
          'Content-Type': 'application/json',
          'Prefer': 'return=representation'
        },
        body: JSON.stringify({
          number: nextNumber,
          url: url,
          status: 'No'
        })
      }
    );

    if (!response.ok) {
      const error = await response.text();
      throw new Error(`Failed to add URL: ${response.statusText} - ${error}`);
    }

    const result = await response.json();
    console.log('Successfully added URL to database:', result);

    return { success: true, number: nextNumber };
  } catch (error) {
    console.error('Error adding URL to database:', error);
    return { success: false, error: error.message };
  }
}

// Test Supabase connection
async function testConnection() {
  try {
    const config = await loadSupabaseConfig();
    
    if (!config.url || !config.key) {
      throw new Error('Supabase URL and API Key are required');
    }

    // Try to query the urls table
    const response = await fetch(
      `${config.url}/rest/v1/urls?select=count&limit=1`,
      {
        headers: {
          'apikey': config.key,
          'Authorization': `Bearer ${config.key}`,
          'Content-Type': 'application/json'
        }
      }
    );

    if (!response.ok) {
      const error = await response.text();
      throw new Error(`Connection failed: ${response.statusText} - ${error}`);
    }

    return { success: true, message: 'Connected to Supabase successfully!' };
  } catch (error) {
    console.error('Connection test failed:', error);
    return { success: false, error: error.message };
  }
}

// Listen for keyboard shortcut (F9 or Ctrl+Shift+U)
chrome.commands.onCommand.addListener(async (command) => {
  if (command === 'copy-url-to-sheet') {
    try {
      const [tab] = await chrome.tabs.query({ active: true, currentWindow: true });

      if (tab && tab.url) {
        const result = await addUrlToDatabase(tab.url);

        if (result.success) {
          // Show success notification
          chrome.notifications.create({
            type: 'basic',
            iconUrl: 'icon.png',
            title: 'LinkCopier',
            message: `✓ URL #${result.number} saved to database!`
          });
        } else {
          // Show error notification
          chrome.notifications.create({
            type: 'basic',
            iconUrl: 'icon.png',
            title: 'LinkCopier Error',
            message: `✗ Failed: ${result.error}`
          });
        }
      }
    } catch (error) {
      console.error('Error in command handler:', error);
    }
  }
});

// Handle messages from popup and options page
chrome.runtime.onMessage.addListener((request, sender, sendResponse) => {
  if (request.action === 'addUrl') {
    addUrlToDatabase(request.url).then(result => {
      sendResponse(result);
    });
    return true; // Keep channel open for async response
  }

  if (request.action === 'testConnection') {
    testConnection().then(result => {
      sendResponse(result);
    });
    return true;
  }
});

// Initialize on install
chrome.runtime.onInstalled.addListener(async () => {
  console.log('LinkCopier extension installed (Supabase version)');
});

