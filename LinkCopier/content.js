// Content script to handle F9 key press
// Note: Chrome extensions handle keyboard shortcuts via commands API in manifest
// This file is kept minimal as the main logic is in background.js

console.log('LinkCopier content script loaded');

// Listen for messages from background script
chrome.runtime.onMessage.addListener((request, sender, sendResponse) => {
  if (request.action === 'ping') {
    sendResponse({ status: 'ready' });
    return true;
  }

  if (request.action === 'copyToClipboard') {
    // Copy data to clipboard in content script context
    navigator.clipboard.writeText(request.data)
      .then(() => {
        sendResponse({ success: true });
      })
      .catch((error) => {
        sendResponse({ success: false, error: error.message });
      });
    return true; // Keep channel open for async response
  }
});

