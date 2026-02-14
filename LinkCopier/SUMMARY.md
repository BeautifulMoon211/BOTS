# LinkCopier Extension - Summary

## 🎯 What It Does

LinkCopier is a Chrome extension that automatically copies the current browser tab's URL to a Google Sheet when you press **F9**.

## ✨ Key Features

1. **F9 Hotkey** - Press F9 to instantly copy the current URL
2. **Auto-numbering** - Each URL gets an auto-incremented number (1, 2, 3, ...)
3. **Status Tracking** - Dropdown column with "Yes" (Green) / "No" (Red, default)
4. **Centered Text** - All cells are automatically centered
5. **Real-time Sync** - URLs are added immediately to the Google Sheet
6. **Notifications** - Get instant feedback when URLs are added
7. **Configurable** - Set your own Google Sheet URL in settings

## 📁 Files Created

### Core Extension Files
- **manifest.json** - Extension configuration and permissions
- **background.js** - Main logic for URL copying and Google Sheets integration
- **content.js** - Content script (minimal, for future enhancements)
- **popup.html/js** - Extension popup interface
- **options.html/js** - Settings page for configuring Google Sheet URL
- **icon.png** - Extension icon (128x128)

### Documentation
- **README.md** - Complete user documentation
- **INSTALL.md** - Step-by-step installation guide
- **SUMMARY.md** - This file

### Utilities
- **setup-sheet.js** - Script to initialize Google Sheet with proper formatting
- **test.html** - Test page to verify extension functionality
- **generate_icon.py** - Python script to generate the extension icon
- **create-icon.html** - HTML-based icon generator

## 🔧 Technical Details

### Google Sheets Integration
- Uses Google Sheets API v4
- OAuth 2.0 authentication via Chrome Identity API
- Batch operations for efficient formatting
- Conditional formatting for status colors

### Permissions Required
- `tabs` - Access current tab URL
- `storage` - Store user settings (Google Sheet URL)
- `activeTab` - Get active tab information
- `identity` - OAuth authentication
- `notifications` - Show success/error notifications

### Google Sheet Structure

| Column | Content | Format |
|--------|---------|--------|
| A | Number | Auto-incremented (1, 2, 3...) |
| B | URL | The copied URL |
| C | Status | Dropdown: "Yes" (Green) / "No" (Red) |

All cells are centered horizontally and vertically.

### Default Google Sheet
```
https://docs.google.com/spreadsheets/d/1p7n7mjurMZWstJKEeQw7gMici6VOOdlScVB6Jav0HVM
```

## 🚀 Quick Start

1. **Load Extension**
   - Open `chrome://extensions/`
   - Enable Developer mode
   - Load unpacked → Select LinkCopier folder

2. **Set Up OAuth** (See INSTALL.md for details)
   - Create Google Cloud project
   - Enable Google Sheets API
   - Create OAuth credentials
   - Update manifest.json with Client ID

3. **Test It**
   - Navigate to any webpage
   - Press F9
   - Check your Google Sheet!

## 🎨 Customization

### Change Keyboard Shortcut
1. Go to `chrome://extensions/shortcuts`
2. Find LinkCopier
3. Change the shortcut key

### Change Google Sheet
1. Click extension icon
2. Click "⚙️ Settings"
3. Enter new Google Sheet URL
4. Save

### Modify Status Colors
Edit `background.js` in the `formatNewRow` function to change:
- Background colors (RGB values)
- Text colors
- Font weight

## 🔍 How It Works

1. User presses F9
2. Chrome commands API triggers `copy-url-to-sheet` command
3. Background script gets current tab URL
4. Script authenticates with Google Sheets API (OAuth)
5. Script reads sheet to get next row number
6. Script appends new row with [Number, URL, "No"]
7. Script formats the row (center align, dropdown, colors)
8. User gets notification of success/failure

## 📊 Google Sheets API Calls

For each URL copy:
1. **GET** - Read column A to get next number
2. **POST** - Append new row with data
3. **POST** - Batch update to format row (alignment, dropdown, colors)

## 🛠️ Troubleshooting

### Common Issues

1. **"Failed to add URL"**
   - Check OAuth setup
   - Verify Google Sheets API is enabled
   - Check sheet permissions

2. **No notification appears**
   - Check browser notification settings
   - Look for errors in console (F12)

3. **Extension ID changed**
   - Update ID in Google Cloud Console
   - Reload extension

### Debug Mode
1. Open `chrome://extensions/`
2. Find LinkCopier
3. Click "service worker" to see background script console
4. Check for errors

## 🔐 Security & Privacy

- Extension only accesses Google Sheets with user permission
- No data is collected or stored externally
- All operations happen in your browser
- OAuth tokens are managed by Chrome
- Only specified Google Sheet is accessed

## 🎯 Use Cases

- **Research** - Collect URLs while browsing
- **Shopping** - Save product links for later
- **Job Hunting** - Track job posting URLs
- **Content Curation** - Gather articles/resources
- **Team Collaboration** - Share URLs with team via shared sheet
- **Link Management** - Organize bookmarks in spreadsheet format

## 📈 Future Enhancements

Potential features to add:
- Custom column names
- Multiple sheet support
- Tags/categories
- Automatic title fetching
- Duplicate URL detection
- Export to CSV
- Keyboard shortcut customization in UI
- Dark mode for popup/options

## 📝 Notes

- The extension uses Manifest V3 (latest Chrome extension format)
- Compatible with Chrome, Edge, and other Chromium browsers
- Requires active internet connection for Google Sheets API
- First use requires OAuth authorization
- Sheet must be accessible to the authenticated Google account

## 🤝 Contributing

To modify the extension:
1. Edit the relevant files
2. Reload extension in `chrome://extensions/`
3. Test changes
4. Update documentation if needed

## 📄 License

MIT License - Free to use and modify

---

**Created:** 2026-02-14
**Version:** 1.0.0
**Manifest Version:** 3

