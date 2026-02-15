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
    console.log('Data length:', request.data?.length, 'chars');

    // Try multiple methods in order of preference
    copyToClipboardMultiMethod(request.data)
      .then((success) => {
        console.log('Copy result:', success);
        sendResponse({ success: success });
      })
      .catch((error) => {
        console.error('All copy methods failed:', error);
        sendResponse({ success: false, error: error.message });
      });

    return true; // Keep channel open for async response
  }
});

// Try multiple clipboard methods
async function copyToClipboardMultiMethod(text) {
  // Method 1: Try modern Clipboard API (might trigger permission prompt)
  if (navigator.clipboard && navigator.clipboard.writeText) {
    try {
      await navigator.clipboard.writeText(text);
      console.log('✓ Method 1 succeeded: Clipboard API');
      return true;
    } catch (error) {
      console.log('✗ Method 1 failed (Clipboard API):', error.message);
      // Continue to next method
    }
  }

  // Method 2: Try execCommand with textarea
  try {
    const success = copyToClipboardFallback(text);
    if (success) {
      console.log('✓ Method 2 succeeded: execCommand');
      return true;
    }
    console.log('✗ Method 2 failed: execCommand returned false');
  } catch (error) {
    console.log('✗ Method 2 failed (execCommand):', error.message);
  }

  // Method 3: Try with contentEditable div
  try {
    const success = copyUsingContentEditable(text);
    if (success) {
      console.log('✓ Method 3 succeeded: contentEditable');
      return true;
    }
    console.log('✗ Method 3 failed: contentEditable returned false');
  } catch (error) {
    console.log('✗ Method 3 failed (contentEditable):', error.message);
  }

  // All methods failed
  throw new Error('All clipboard methods failed');
}

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

    // Make textarea visible but off-screen for better compatibility
    textarea.style.position = 'absolute';
    textarea.style.left = '-9999px';
    textarea.style.top = '0';

    // Ensure textarea is editable and visible to the browser
    textarea.setAttribute('readonly', '');
    textarea.style.opacity = '0';
    textarea.style.pointerEvents = 'none';

    document.body.appendChild(textarea);

    // Select the text
    textarea.select();
    textarea.setSelectionRange(0, text.length);

    // Try to copy
    let successful = false;
    try {
      successful = document.execCommand('copy');
    } catch (err) {
      console.error('execCommand threw error:', err);
    }

    document.body.removeChild(textarea);

    if (successful) {
      console.log('Successfully copied using execCommand method');
      return true;
    } else {
      console.error('execCommand copy method failed - command returned false');
      console.log('Text length:', text.length, 'First 100 chars:', text.substring(0, 100));
      return false;
    }
  } catch (error) {
    console.error('Error in copy method:', error);
    return false;
  }
}

// Method 3: Using contentEditable div
function copyUsingContentEditable(text) {
  try {
    if (!document.body) {
      console.error('document.body is not available');
      return false;
    }

    const div = document.createElement('div');
    div.contentEditable = 'true';
    div.textContent = text;
    div.style.position = 'absolute';
    div.style.left = '-9999px';
    div.style.opacity = '0';

    document.body.appendChild(div);

    // Select all content
    const range = document.createRange();
    range.selectNodeContents(div);
    const selection = window.getSelection();
    selection.removeAllRanges();
    selection.addRange(range);

    // Try to copy
    let successful = false;
    try {
      successful = document.execCommand('copy');
    } catch (err) {
      console.error('execCommand in contentEditable threw error:', err);
    }

    // Clean up
    selection.removeAllRanges();
    document.body.removeChild(div);

    return successful;
  } catch (error) {
    console.error('Error in contentEditable copy method:', error);
    return false;
  }
}

