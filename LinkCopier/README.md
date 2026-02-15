# 🔗 LinkCopier - Real-time URL Tracker

> **Press Ctrl+Shift+U (or F9 after setup) to instantly save any URL to your free Supabase database!**

A Chrome extension that automatically saves the current tab's URL to a Supabase database with a keyboard shortcut. Features auto-numbering, status tracking, and real-time synchronization. **100% free - no payment required!**

[![Chrome Extension](https://img.shields.io/badge/Chrome-Extension-4285f4?logo=google-chrome&logoColor=white)](chrome://extensions/)
[![Manifest V3](https://img.shields.io/badge/Manifest-V3-34a853)](manifest.json)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## ✨ Features

- 🎯 **F9 Hotkey**: Press F9 to instantly save the current URL to your database
- 🔢 **Auto-numbering**: Each URL gets an auto-incremented number (1, 2, 3...)
- 📊 **Status Tracking**: Track URLs with "Yes" or "No" status
- 💾 **Free Database**: Uses Supabase (500MB free, no credit card required!)
- 🔄 **Real-time Sync**: Changes appear instantly in your database
- 🌐 **Web Dashboard**: View and edit your data online at supabase.com
- 🚫 **No OAuth**: Simple API key authentication (no Google Cloud setup!)
- 🔔 **Notifications**: Get instant feedback when URLs are saved

## Installation

### 1. Set up Google Cloud Project (Required for OAuth)

1. Go to [Google Cloud Console](https://console.cloud.google.com/)
2. Create a new project or select an existing one
3. Enable the **Google Sheets API**:
   - Go to "APIs & Services" > "Library"
   - Search for "Google Sheets API"
   - Click "Enable"
4. Create OAuth 2.0 credentials:
   - Go to "APIs & Services" > "Credentials"
   - Click "Create Credentials" > "OAuth client ID"
   - Choose "Chrome Extension" as application type
   - Add your extension ID (you'll get this after loading the extension)
   - Download the credentials

### 2. Install the Extension

1. Open Chrome and navigate to `chrome://extensions/`
2. Enable **Developer mode** (toggle in top right)
3. Click **Load unpacked**
4. Select the `LinkCopier` folder
5. Copy the **Extension ID** shown

### 3. Configure OAuth

1. Go back to Google Cloud Console > Credentials
2. Edit your OAuth client ID
3. Add the extension ID to the authorized origins
4. Update `manifest.json` with your OAuth client ID:
   ```json
   "oauth2": {
     "client_id": "YOUR_CLIENT_ID.apps.googleusercontent.com",
     "scopes": [
       "https://www.googleapis.com/auth/spreadsheets"
     ]
   }
   ```

### 4. Prepare Your Google Sheet

1. Create a new Google Sheet or use the default one
2. Add headers in the first row:
   - **A1**: Number
   - **B1**: URL
   - **C1**: Status
3. Format the header row (optional):
   - Bold text
   - Center alignment
   - Background color

## Usage

### Quick Start

1. Navigate to any webpage
2. Press **F9** on your keyboard
3. The URL will be automatically added to your Google Sheet!

### Alternative Methods

- Click the extension icon and press "Copy Current URL Now"
- Use the popup for quick access

### Settings

1. Click the extension icon
2. Click "⚙️ Settings"
3. Enter your Google Sheet URL
4. Click "Save Settings"
5. Use "Test Connection" to verify it works

## Google Sheet Structure

| Number | URL | Status |
|--------|-----|--------|
| 1 | https://example.com | No |
| 2 | https://google.com | Yes |
| 3 | https://github.com | No |

- **Number**: Auto-incremented (1, 2, 3, ...)
- **URL**: The copied URL from the browser tab
- **Status**: Dropdown with "Yes" (Green) or "No" (Red, default)
- **All cells**: Centered alignment

## Default Google Sheet

The extension comes pre-configured with this Google Sheet:
```
https://docs.google.com/spreadsheets/d/1p7n7mjurMZWstJKEeQw7gMici6VOOdlScVB6Jav0HVM
```

You can change this in the settings.

## Keyboard Shortcut

- **F9**: Copy current URL to Google Sheet

To change the keyboard shortcut:
1. Go to `chrome://extensions/shortcuts`
2. Find "LinkCopier"
3. Click the pencil icon to customize

## Troubleshooting

### "Failed to add URL" error

1. Make sure you've authorized the extension to access Google Sheets
2. Check that your Google Sheet URL is correct
3. Verify the sheet is not private/restricted
4. Try the "Test Connection" button in settings

### OAuth not working

1. Verify your OAuth client ID is correct in `manifest.json`
2. Make sure the Google Sheets API is enabled
3. Check that your extension ID is added to authorized origins
4. Try removing and re-adding the extension

### URLs not appearing in sheet

1. Check that the sheet has the correct permissions
2. Verify you're looking at the right sheet
3. Check the browser console for errors (F12)

## Files

- `manifest.json` - Extension configuration
- `background.js` - Main logic for URL copying and Google Sheets integration
- `content.js` - Content script (minimal)
- `popup.html/js` - Extension popup interface
- `options.html/js` - Settings page
- `icon.png` - Extension icon
- `README.md` - This file

## Privacy

This extension:
- Only accesses Google Sheets with your explicit permission
- Does not collect or store any personal data
- Only sends URLs to your specified Google Sheet
- Runs entirely in your browser

## License

MIT License - Feel free to modify and distribute

## Support

For issues or questions, please check the browser console (F12) for error messages.

