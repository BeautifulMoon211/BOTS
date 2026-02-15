// Content script to handle F9 key press
// Note: Chrome extensions handle keyboard shortcuts via commands API in manifest
// This file is kept minimal as the main logic is in background.js

console.log('LinkCopier content script loaded');

// Listen for messages from background script
chrome.runtime.onMessage.addListener((request, _sender, sendResponse) => {
  if (request.action === 'ping') {
    sendResponse({ status: 'ready' });
    return true;
  }

  if (request.action === 'copyToClipboard') {
    console.log('Attempting to copy to clipboard');

    // Use execCommand method to avoid permission prompts
    // The modern Clipboard API (navigator.clipboard) triggers permission prompts
    // even with clipboardWrite permission, so we use the legacy method
    const success = copyToClipboardFallback(request.data);
    sendResponse({ success: success });

    return true; // Keep channel open for async response
  }
});

// Fallback method using textarea and execCommand
// This method doesn't trigger permission prompts
function copyToClipboardFallback(text) {
  try {
    // Check if document.body exists
    if (!document.body) {
      console.error('document.body is not available yet');
      return false;
    }

    const textarea = document.createElement('textarea');
    textarea.value = text;
    textarea.style.position = 'fixed';
    textarea.style.top = '0';
    textarea.style.left = '0';
    textarea.style.width = '1px';
    textarea.style.height = '1px';
    textarea.style.padding = '0';
    textarea.style.border = 'none';
    textarea.style.outline = 'none';
    textarea.style.boxShadow = 'none';
    textarea.style.background = 'transparent';

    document.body.appendChild(textarea);
    textarea.focus();
    textarea.select();

    const successful = document.execCommand('copy');
    document.body.removeChild(textarea);

    if (successful) {
      console.log('Successfully copied using execCommand method');
      return true;
    } else {
      console.error('execCommand copy method failed - command returned false');
      return false;
    }
  } catch (error) {
    console.error('Error in copy method:', error);
    return false;
  }
}

