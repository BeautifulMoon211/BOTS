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

// Open settings button
document.getElementById('openSettings').addEventListener('click', () => {
  chrome.runtime.openOptionsPage();
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

