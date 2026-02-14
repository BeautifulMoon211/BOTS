# LinkCopier - File Structure

## 📁 Complete File List

### Core Extension Files (Required)

| File | Purpose | Size |
|------|---------|------|
| **manifest.json** | Extension configuration, permissions, OAuth setup | ~1 KB |
| **background.js** | Main logic: URL capture, Google Sheets API integration | ~8 KB |
| **content.js** | Content script (minimal, for future use) | ~0.5 KB |
| **popup.html** | Extension popup user interface | ~2 KB |
| **popup.js** | Popup functionality and event handlers | ~1 KB |
| **options.html** | Settings page user interface | ~4 KB |
| **options.js** | Settings page logic and sheet connection test | ~3 KB |
| **icon.png** | Extension icon (128x128 PNG) | ~1.2 KB |

**Total Core Size:** ~21 KB

---

### Documentation Files

| File | Purpose | Size |
|------|---------|------|
| **README.md** | Complete user documentation and features | ~6 KB |
| **INSTALL.md** | Detailed step-by-step installation guide | ~7 KB |
| **QUICKSTART.md** | 5-minute quick start guide | ~5 KB |
| **SUMMARY.md** | Technical overview and architecture | ~5 KB |
| **CHECKLIST.md** | Installation verification checklist | ~6 KB |
| **FILES.md** | This file - complete file structure | ~2 KB |

**Total Documentation:** ~31 KB

---

### Utility Files

| File | Purpose | Size |
|------|---------|------|
| **setup-sheet.js** | Initialize Google Sheet with formatting | ~6 KB |
| **test.html** | Test page to verify extension functionality | ~4 KB |
| **generate_icon.py** | Python script to generate extension icon | ~1.5 KB |
| **create-icon.html** | HTML-based icon generator (alternative) | ~1.5 KB |

**Total Utilities:** ~13 KB

---

## 📂 Directory Structure

```
LinkCopier/
├── manifest.json          ← Extension configuration
├── background.js          ← Main background service worker
├── content.js             ← Content script
├── popup.html             ← Popup UI
├── popup.js               ← Popup logic
├── options.html           ← Settings page UI
├── options.js             ← Settings page logic
├── icon.png               ← Extension icon
├── setup-sheet.js         ← Sheet initialization script
├── test.html              ← Test page
├── generate_icon.py       ← Icon generator (Python)
├── create-icon.html       ← Icon generator (HTML)
├── README.md              ← Main documentation
├── INSTALL.md             ← Installation guide
├── QUICKSTART.md          ← Quick start guide
├── SUMMARY.md             ← Technical summary
├── CHECKLIST.md           ← Setup checklist
└── FILES.md               ← This file
```

---

## 🔧 File Dependencies

### manifest.json references:
- background.js (service worker)
- content.js (content script)
- popup.html (popup page)
- options.html (options page)
- icon.png (extension icon)

### popup.html loads:
- popup.js

### options.html loads:
- options.js

### Standalone files:
- setup-sheet.js (can be run independently)
- test.html (can be opened in browser)
- generate_icon.py (run with Python)
- create-icon.html (open in browser)

---

## 📝 File Descriptions

### manifest.json
- Defines extension metadata (name, version, description)
- Specifies permissions (tabs, storage, identity, notifications)
- Configures OAuth 2.0 client ID
- Sets up F9 keyboard command
- Declares background service worker
- Defines content scripts

### background.js
- Handles F9 command events
- Manages OAuth authentication
- Interacts with Google Sheets API
- Reads current row numbers
- Appends new URLs to sheet
- Formats rows (alignment, dropdown, colors)
- Shows success/error notifications
- Handles messages from popup

### content.js
- Minimal content script
- Listens for messages from background
- Can be extended for future features

### popup.html/js
- User interface when clicking extension icon
- "Copy Current URL Now" button
- "Settings" button
- Status messages
- Sends messages to background script

### options.html/js
- Settings page for configuration
- Google Sheet URL input
- Save settings functionality
- Test connection feature
- Validates sheet URL format
- Stores settings in chrome.storage

### icon.png
- 128x128 pixel PNG image
- Blue gradient background
- Chain link symbol
- "F9" text label
- Used in toolbar, popup, and options

### setup-sheet.js
- Initializes Google Sheet with proper formatting
- Adds headers (Number, URL, Status)
- Sets column widths
- Applies conditional formatting
- Freezes header row
- Can be run from browser console

### test.html
- Test page for extension functionality
- Shows current URL
- Lists test URLs to try
- Instructions for testing
- Links to Google Sheet
- Visual feedback

### generate_icon.py
- Python script using Pillow library
- Generates icon.png programmatically
- Creates chain link graphic
- Adds F9 text
- Outputs 128x128 PNG

### create-icon.html
- HTML Canvas-based icon generator
- Alternative to Python script
- Right-click to save as PNG
- Same design as generate_icon.py

---

## 🎯 Minimum Required Files

To run the extension, you need:

1. ✅ manifest.json
2. ✅ background.js
3. ✅ content.js
4. ✅ popup.html
5. ✅ popup.js
6. ✅ options.html
7. ✅ options.js
8. ✅ icon.png

**All other files are documentation or utilities.**

---

## 📊 File Sizes Summary

| Category | Files | Total Size |
|----------|-------|------------|
| Core Extension | 8 files | ~21 KB |
| Documentation | 6 files | ~31 KB |
| Utilities | 4 files | ~13 KB |
| **TOTAL** | **18 files** | **~65 KB** |

---

## 🔄 File Modification Guide

### To change functionality:
- Edit **background.js** for core logic
- Edit **popup.js** for popup behavior
- Edit **options.js** for settings behavior

### To change appearance:
- Edit **popup.html** for popup UI
- Edit **options.html** for settings UI
- Edit **icon.png** for extension icon

### To change configuration:
- Edit **manifest.json** for permissions, commands, OAuth

### To update documentation:
- Edit **README.md** for user docs
- Edit **INSTALL.md** for installation steps
- Edit **SUMMARY.md** for technical details

---

## 🚀 Getting Started

1. **Read first:** QUICKSTART.md (5 minutes)
2. **Install:** Follow INSTALL.md
3. **Verify:** Use CHECKLIST.md
4. **Learn more:** Read README.md and SUMMARY.md

---

## 📞 File-Specific Help

| Issue | Check This File |
|-------|----------------|
| Installation problems | INSTALL.md, CHECKLIST.md |
| How to use | README.md, QUICKSTART.md |
| Technical details | SUMMARY.md |
| OAuth setup | INSTALL.md |
| Sheet formatting | setup-sheet.js |
| Testing | test.html |

---

**Last Updated:** 2026-02-14
**Version:** 1.0.0
