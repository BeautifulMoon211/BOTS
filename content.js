// Content script to handle F9 key press
// Note: Chrome extensions handle keyboard shortcuts via commands API in manifest
// This file is kept minimal as the main logic is in background.js

console.log('LinkCopier content script loaded');

// Listen for messages from background script if needed
chrome.runtime.onMessage.addListener((request, sender, sendResponse) => {
  if (request.action === 'ping') {
    sendResponse({ status: 'ready' });
  }
  return true;
});

