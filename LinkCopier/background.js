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

// Check if URL already exists in database
async function urlExists(url) {
  try {
    const config = await loadSupabaseConfig();

    // Query for exact URL match
    const response = await fetch(
      `${config.url}/rest/v1/urls?url=eq.${encodeURIComponent(url)}&select=id,number`,
      {
        headers: {
          'apikey': config.key,
          'Authorization': `Bearer ${config.key}`,
          'Content-Type': 'application/json'
        }
      }
    );

    if (!response.ok) {
      throw new Error(`Failed to check URL: ${response.statusText}`);
    }

    const data = await response.json();

    if (data.length > 0) {
      return { exists: true, number: data[0].number };
    }

    return { exists: false };
  } catch (error) {
    console.error('Error checking URL existence:', error);
    return { exists: false };
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

    // Check if URL already exists
    const existsCheck = await urlExists(url);
    if (existsCheck.exists) {
      return {
        success: false,
        error: `URL already exists as #${existsCheck.number}`,
        duplicate: true,
        existingNumber: existsCheck.number
      };
    }

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
          copied: 'No'
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

// Export uncopied rows to clipboard
async function exportUncopiedRows() {
  try {
    const configured = await isConfigured();
    if (!configured) {
      throw new Error('Supabase not configured. Please set up in Settings.');
    }

    const config = await loadSupabaseConfig();

    // Get all rows where copied = 'No'
    const response = await fetch(
      `${config.url}/rest/v1/urls?copied=eq.No&select=id,number,url&order=number.asc`,
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
      throw new Error(`Failed to fetch rows: ${response.statusText} - ${error}`);
    }

    const rows = await response.json();

    if (rows.length === 0) {
      return { success: false, error: 'No uncopied rows found', count: 0 };
    }

    // Format data as TSV (Tab-Separated Values) for easy paste into Google Sheets
    const tsvData = rows.map(row => `${row.number}\t${row.url}`).join('\n');

    // Return data with IDs for later update (after successful clipboard copy)
    const ids = rows.map(row => row.id);

    console.log(`Fetched ${rows.length} uncopied rows`);

    return { success: true, count: rows.length, data: tsvData, ids: ids };
  } catch (error) {
    console.error('Error exporting rows:', error);
    return { success: false, error: error.message };
  }
}

// Mark rows as copied in database
async function markRowsAsCopied(ids) {
  try {
    const config = await loadSupabaseConfig();

    const updateResponse = await fetch(
      `${config.url}/rest/v1/urls?id=in.(${ids.join(',')})`,
      {
        method: 'PATCH',
        headers: {
          'apikey': config.key,
          'Authorization': `Bearer ${config.key}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          copied: 'Yes'
        })
      }
    );

    if (!updateResponse.ok) {
      const error = await updateResponse.text();
      throw new Error(`Failed to update rows: ${updateResponse.statusText} - ${error}`);
    }

    console.log(`Successfully marked ${ids.length} rows as copied`);
    return { success: true };
  } catch (error) {
    console.error('Error marking rows as copied:', error);
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

// Show visual feedback on the page
async function showPageNotification(tabId, message, isSuccess) {
  try {
    await chrome.scripting.executeScript({
      target: { tabId: tabId },
      func: (msg, success) => {
        // Remove any existing notification
        const existing = document.getElementById('linkcopy-notification');
        if (existing) existing.remove();

        // Create notification element
        const notification = document.createElement('div');
        notification.id = 'linkcopy-notification';
        notification.style.cssText = `
          position: fixed;
          top: 20px;
          right: 20px;
          background: ${success ? '#4caf50' : '#f44336'};
          color: white;
          padding: 16px 24px;
          border-radius: 8px;
          box-shadow: 0 4px 12px rgba(0,0,0,0.3);
          z-index: 999999;
          font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
          font-size: 15px;
          font-weight: 500;
          animation: slideIn 0.3s ease-out;
        `;
        notification.textContent = msg;

        // Add animation
        const style = document.createElement('style');
        style.textContent = `
          @keyframes slideIn {
            from { transform: translateX(400px); opacity: 0; }
            to { transform: translateX(0); opacity: 1; }
          }
          @keyframes slideOut {
            from { transform: translateX(0); opacity: 1; }
            to { transform: translateX(400px); opacity: 0; }
          }
        `;
        document.head.appendChild(style);
        document.body.appendChild(notification);

        // Auto-remove after 3 seconds
        setTimeout(() => {
          notification.style.animation = 'slideOut 0.3s ease-out';
          setTimeout(() => notification.remove(), 300);
        }, 3000);
      },
      args: [message, isSuccess]
    });
  } catch (error) {
    console.log('Could not inject notification (page may not allow scripts):', error);
  }
}

// Update extension badge
function updateBadge(text, color) {
  chrome.action.setBadgeText({ text: text });
  chrome.action.setBadgeBackgroundColor({ color: color });

  // Clear badge after 3 seconds
  setTimeout(() => {
    chrome.action.setBadgeText({ text: '' });
  }, 3000);
}

// Listen for keyboard shortcut (F9 or Ctrl+Shift+U)
chrome.commands.onCommand.addListener(async (command) => {
  if (command === 'copy-url-to-sheet') {
    try {
      const [tab] = await chrome.tabs.query({ active: true, currentWindow: true });

      if (tab && tab.url) {
        const result = await addUrlToDatabase(tab.url);

        if (result.success) {
          // Show success notification (system)
          chrome.notifications.create({
            type: 'basic',
            iconUrl: 'icon.png',
            title: '✓ LinkCopier Success',
            message: `URL #${result.number} saved to database!`,
            priority: 2
          });

          // Show on-page notification
          await showPageNotification(tab.id, `✓ URL #${result.number} saved!`, true);

          // Update badge
          updateBadge('✓', '#4caf50');

        } else {
          // Check if it's a duplicate
          if (result.duplicate) {
            // Show warning notification for duplicate (system)
            chrome.notifications.create({
              type: 'basic',
              iconUrl: 'icon.png',
              title: '⚠ URL Already Exists',
              message: `This URL is already saved as #${result.existingNumber}`,
              priority: 1
            });

            // Show on-page notification (warning style)
            await showPageNotification(tab.id, `⚠ Already saved as #${result.existingNumber}`, false);

            // Update badge with warning
            updateBadge('⚠', '#ff9800');
          } else {
            // Show error notification (system)
            chrome.notifications.create({
              type: 'basic',
              iconUrl: 'icon.png',
              title: '✗ LinkCopier Error',
              message: `Failed: ${result.error}`,
              priority: 2
            });

            // Show on-page notification
            await showPageNotification(tab.id, `✗ Failed: ${result.error}`, false);

            // Update badge
            updateBadge('✗', '#f44336');
          }
        }
      }
    } catch (error) {
      console.error('Error in command handler:', error);

      // Show error notification
      chrome.notifications.create({
        type: 'basic',
        iconUrl: 'icon.png',
        title: '✗ LinkCopier Error',
        message: `Error: ${error.message}`,
        priority: 2
      });
    }
  }

  if (command === 'export-to-clipboard') {
    try {
      const result = await exportUncopiedRows();

      if (result.success) {
        // Get active tab to send clipboard data
        const [tab] = await chrome.tabs.query({ active: true, currentWindow: true });

        if (tab && tab.id) {
          try {
            // Send data to content script to copy to clipboard
            const response = await chrome.tabs.sendMessage(
              tab.id,
              { action: 'copyToClipboard', data: result.data }
            );

            if (response && response.success) {
              // Clipboard copy succeeded - now mark rows as copied in database
              await markRowsAsCopied(result.ids);

              // Show success notification
              chrome.notifications.create({
                type: 'basic',
                iconUrl: 'icon.png',
                title: '✓ Export Success',
                message: `${result.count} rows copied to clipboard!`,
                priority: 2
              });

              // Update badge
              updateBadge('✓', '#4caf50');
            } else {
              // Clipboard copy failed - don't update database
              throw new Error('Content script failed to copy');
            }
          } catch (clipboardError) {
            console.error('Clipboard error:', clipboardError);

            // Show error notification with helpful message
            chrome.notifications.create({
              type: 'basic',
              iconUrl: 'icon.png',
              title: '✗ Clipboard Error',
              message: 'Failed to copy to clipboard. Make sure you are on a regular webpage (not chrome:// page).',
              priority: 2
            });

            // Update badge
            updateBadge('✗', '#f44336');
          }
        } else {
          throw new Error('No active tab found');
        }
      } else {
        // Show error notification
        chrome.notifications.create({
          type: 'basic',
          iconUrl: 'icon.png',
          title: '✗ Export Error',
          message: `Failed: ${result.error}`,
          priority: 2
        });

        // Update badge
        updateBadge('✗', '#f44336');
      }
    } catch (error) {
      console.error('Error in export command handler:', error);

      // Show error notification
      chrome.notifications.create({
        type: 'basic',
        iconUrl: 'icon.png',
        title: '✗ Export Error',
        message: `Error: ${error.message}`,
        priority: 2
      });

      // Update badge
      updateBadge('✗', '#f44336');
    }
  }
});

// Handle messages from popup and options page
chrome.runtime.onMessage.addListener((request, _sender, sendResponse) => {
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

  if (request.action === 'exportUncopied') {
    exportUncopiedRows().then(result => {
      sendResponse(result);
    });
    return true;
  }

  if (request.action === 'markAsCopied') {
    markRowsAsCopied(request.ids).then(result => {
      sendResponse(result);
    });
    return true;
  }
});

// Initialize on install
chrome.runtime.onInstalled.addListener(async () => {
  console.log('LinkCopier extension installed (Supabase version)');
});

