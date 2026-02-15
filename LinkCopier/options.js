// Options page script for Supabase version

// Load saved settings
document.addEventListener('DOMContentLoaded', async () => {
  const result = await chrome.storage.sync.get(['supabaseUrl', 'supabaseKey']);
  document.getElementById('supabaseUrl').value = result.supabaseUrl || '';
  document.getElementById('supabaseKey').value = result.supabaseKey || '';
});

// Save settings
document.getElementById('save').addEventListener('click', async () => {
  const supabaseUrl = document.getElementById('supabaseUrl').value.trim();
  const supabaseKey = document.getElementById('supabaseKey').value.trim();
  
  if (!supabaseUrl || !supabaseKey) {
    showStatus('Please enter both Supabase URL and API Key', 'error');
    return;
  }

  // Validate URL format
  if (!supabaseUrl.match(/https:\/\/[a-zA-Z0-9-]+\.supabase\.co/)) {
    showStatus('Invalid Supabase URL format. Should be: https://xxxxx.supabase.co', 'error');
    return;
  }

  await chrome.storage.sync.set({ supabaseUrl, supabaseKey });
  showStatus('✓ Settings saved successfully!', 'success');
});

// Test connection
document.getElementById('test').addEventListener('click', async () => {
  const supabaseUrl = document.getElementById('supabaseUrl').value.trim();
  const supabaseKey = document.getElementById('supabaseKey').value.trim();
  
  if (!supabaseUrl || !supabaseKey) {
    showStatus('Please enter both Supabase URL and API Key first', 'error');
    return;
  }

  try {
    showStatus('Testing connection...', 'success');
    
    // Try to query the urls table
    const response = await fetch(
      `${supabaseUrl}/rest/v1/urls?select=count&limit=1`,
      {
        headers: {
          'apikey': supabaseKey,
          'Authorization': `Bearer ${supabaseKey}`,
          'Content-Type': 'application/json'
        }
      }
    );

    if (response.ok) {
      // Count total URLs
      const countResponse = await fetch(
        `${supabaseUrl}/rest/v1/urls?select=count`,
        {
          headers: {
            'apikey': supabaseKey,
            'Authorization': `Bearer ${supabaseKey}`,
            'Content-Type': 'application/json',
            'Prefer': 'count=exact'
          }
        }
      );
      
      const count = countResponse.headers.get('content-range')?.split('/')[1] || '0';
      showStatus(`✓ Connected successfully! Database has ${count} URLs stored.`, 'success');
    } else {
      const error = await response.text();
      showStatus(`✗ Connection failed: ${response.statusText}. Make sure the 'urls' table exists.`, 'error');
      console.error('Test error:', error);
    }
  } catch (error) {
    showStatus(`✗ Error: ${error.message}`, 'error');
    console.error('Test error:', error);
  }
});

// View database
document.getElementById('viewData').addEventListener('click', async () => {
  const supabaseUrl = document.getElementById('supabaseUrl').value.trim();
  
  if (!supabaseUrl) {
    showStatus('Please save Supabase URL first', 'error');
    return;
  }

  // Extract project ID from URL
  const match = supabaseUrl.match(/https:\/\/([a-zA-Z0-9-]+)\.supabase\.co/);
  if (match) {
    const projectId = match[1];
    const dashboardUrl = `https://supabase.com/dashboard/project/${projectId}/editor`;
    window.open(dashboardUrl, '_blank');
  } else {
    showStatus('Invalid Supabase URL', 'error');
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

