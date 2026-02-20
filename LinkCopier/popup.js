// Popup script

// Copy current URL button
document.getElementById('copyNow').addEventListener('click', async () => {
  try {
    const [tab] = await chrome.tabs.query({ active: true, currentWindow: true });
    
    if (tab && tab.url) {
      showStatus('Copying URL to sheet...', 'success');
      
      // Send message to background script to add URL
      chrome.runtime.sendMessage(
        { action: 'addUrl', url: tab.url },
        (response) => {
          if (response && response.success) {
            showStatus(`✓ URL #${response.number} added!`, 'success');
          } else {
            showStatus(`✗ Error: ${response?.error || 'Unknown error'}`, 'error');
          }
        }
      );
    }
  } catch (error) {
    showStatus(`✗ Error: ${error.message}`, 'error');
  }
});

// Export uncopied rows button
document.getElementById('exportUncopied').addEventListener('click', async () => {
  try {
    showStatus('Exporting uncopied rows...', 'success');

    chrome.runtime.sendMessage(
      { action: 'exportUncopied' },
      async (response) => {
        if (response && response.success) {
          // Copy to clipboard in popup context (where navigator.clipboard works)
          try {
            await navigator.clipboard.writeText(response.data);

            // Clipboard copy succeeded - now mark rows as copied in database
            chrome.runtime.sendMessage(
              { action: 'markAsCopied', ids: response.ids },
              (markResponse) => {
                if (markResponse && markResponse.success) {
                  showStatus(`✓ ${response.count} rows copied to clipboard!`, 'success');
                } else {
                  showStatus(`✓ Copied to clipboard, but failed to update database`, 'warning');
                }
              }
            );
          } catch (clipboardError) {
            // Clipboard copy failed - don't update database
            showStatus(`✗ Clipboard error: ${clipboardError.message}`, 'error');
          }
        } else {
          showStatus(`✗ Error: ${response?.error || 'Unknown error'}`, 'error');
        }
      }
    );
  } catch (error) {
    showStatus(`✗ Error: ${error.message}`, 'error');
  }
});

// Open settings button
document.getElementById('openSettings').addEventListener('click', () => {
  chrome.runtime.openOptionsPage();
});

// Check shortcuts button
document.getElementById('checkShortcuts').addEventListener('click', () => {
  chrome.tabs.create({ url: 'chrome://extensions/shortcuts' });
});

// Show status message
function showStatus(message, type) {
  const statusDiv = document.getElementById('status');
  statusDiv.textContent = message;
  statusDiv.className = `status ${type}`;
  statusDiv.style.display = 'block';
  
  // Auto-hide after 3 seconds
  setTimeout(() => {
    statusDiv.style.display = 'none';
  }, 3000);
}

