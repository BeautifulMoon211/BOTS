# 🔗 LinkCopier Extension - Complete Index

## 📖 What is LinkCopier?

LinkCopier is a Chrome extension that automatically copies the current browser tab's URL to a Google Sheet when you press **F9**. It features auto-numbering, status tracking with color-coded dropdowns, and real-time synchronization.

---

## 🚀 Quick Navigation

### For New Users
1. **Start Here:** [QUICKSTART.md](QUICKSTART.md) - Get up and running in 5 minutes
2. **Detailed Setup:** [INSTALL.md](INSTALL.md) - Step-by-step installation guide
3. **Verify Setup:** [CHECKLIST.md](CHECKLIST.md) - Ensure everything works

### For Learning More
- **User Guide:** [README.md](README.md) - Complete documentation
- **Technical Details:** [SUMMARY.md](SUMMARY.md) - Architecture and implementation
- **File Structure:** [FILES.md](FILES.md) - All files explained

---

## 📚 Documentation Guide

| Document | Purpose | Read Time | Audience |
|----------|---------|-----------|----------|
| **QUICKSTART.md** | Get started fast | 5 min | Everyone |
| **INSTALL.md** | Detailed installation | 10 min | First-time users |
| **CHECKLIST.md** | Verify installation | 5 min | During setup |
| **README.md** | Complete user guide | 15 min | All users |
| **SUMMARY.md** | Technical overview | 10 min | Developers |
| **FILES.md** | File structure | 5 min | Developers |
| **INDEX.md** | This file | 3 min | Everyone |

---

## ✨ Key Features

- ⌨️ **F9 Hotkey** - Press F9 to copy URL instantly
- 🔢 **Auto-numbering** - Each URL gets sequential number (1, 2, 3...)
- ✅ **Status Tracking** - Dropdown with Yes (Green) / No (Red)
- 🎯 **Centered Text** - All cells automatically centered
- ⚡ **Real-time Sync** - URLs appear immediately in Google Sheet
- 🔔 **Notifications** - Instant feedback on success/failure
- ⚙️ **Configurable** - Use your own Google Sheet

---

## 🎯 Use Cases

- 📚 **Research** - Collect article URLs while browsing
- 🛍️ **Shopping** - Save product links for comparison
- 💼 **Job Hunting** - Track job posting URLs
- 📝 **Content Curation** - Gather resources for projects
- 👥 **Team Collaboration** - Share URLs via shared sheet
- 🔖 **Link Management** - Better than traditional bookmarks

---

## 📊 Google Sheet Structure

| Column A | Column B | Column C |
|----------|----------|----------|
| **Number** | **URL** | **Status** |
| 1 | https://example.com | No ⬇️ |
| 2 | https://google.com | Yes ⬇️ |
| 3 | https://github.com | No ⬇️ |

- **Number:** Auto-incremented
- **URL:** Current tab URL
- **Status:** Dropdown (Yes=Green, No=Red)
- **All cells:** Centered alignment

---

## 🔧 Technical Stack

- **Platform:** Chrome Extension (Manifest V3)
- **Language:** JavaScript
- **APIs:** 
  - Chrome Extensions API
  - Chrome Identity API (OAuth 2.0)
  - Google Sheets API v4
- **Storage:** Chrome Storage API
- **UI:** HTML/CSS

---

## 📁 File Overview

### Essential Files (8)
- `manifest.json` - Extension configuration
- `background.js` - Main logic (8 KB)
- `content.js` - Content script
- `popup.html/js` - Extension popup
- `options.html/js` - Settings page
- `icon.png` - Extension icon

### Documentation (7)
- `README.md` - User guide
- `INSTALL.md` - Installation
- `QUICKSTART.md` - Quick start
- `SUMMARY.md` - Technical docs
- `CHECKLIST.md` - Setup checklist
- `FILES.md` - File structure
- `INDEX.md` - This file

### Utilities (4)
- `setup-sheet.js` - Sheet formatter
- `test.html` - Test page
- `generate_icon.py` - Icon generator
- `create-icon.html` - Icon generator (HTML)

**Total:** 19 files, ~65 KB

---

## 🎓 Learning Path

### Beginner
1. Read QUICKSTART.md
2. Follow INSTALL.md
3. Use CHECKLIST.md to verify
4. Test with test.html

### Intermediate
1. Read README.md fully
2. Customize settings
3. Use your own Google Sheet
4. Change keyboard shortcut

### Advanced
1. Read SUMMARY.md
2. Review background.js code
3. Modify functionality
4. Run setup-sheet.js for formatting

---

## 🆘 Troubleshooting

| Problem | Solution | Document |
|---------|----------|----------|
| Installation fails | Check OAuth setup | INSTALL.md |
| F9 doesn't work | Verify permissions | CHECKLIST.md |
| No notification | Check browser settings | README.md |
| URL not added | Test connection | INSTALL.md |
| Extension ID changed | Update Cloud Console | INSTALL.md |

---

## 🔗 Important Links

- **Default Google Sheet:** https://docs.google.com/spreadsheets/d/1p7n7mjurMZWstJKEeQw7gMici6VOOdlScVB6Jav0HVM
- **Google Cloud Console:** https://console.cloud.google.com/
- **Chrome Extensions:** chrome://extensions/
- **Extension Shortcuts:** chrome://extensions/shortcuts

---

## ⚙️ Configuration

### Default Settings
- **Sheet URL:** Pre-configured default sheet
- **Keyboard:** F9 key
- **Status Default:** "No" (Red background)
- **Status Options:** "Yes" (Green) / "No" (Red)

### Customizable
- Google Sheet URL (via Settings)
- Keyboard shortcut (via chrome://extensions/shortcuts)
- Sheet formatting (via setup-sheet.js)

---

## 🎨 Customization Options

1. **Change Sheet URL** - Use your own Google Sheet
2. **Change Hotkey** - Use different keyboard shortcut
3. **Modify Colors** - Edit background.js formatting
4. **Add Columns** - Extend sheet structure
5. **Change Icon** - Replace icon.png

---

## 📞 Getting Help

### Check These First
1. Browser console (F12) for errors
2. Extension service worker console
3. Google Cloud Console logs
4. INSTALL.md troubleshooting section

### Common Issues
- OAuth not configured → See INSTALL.md Step 2
- Extension ID mismatch → Update Cloud Console
- API not enabled → Enable Google Sheets API
- Permission denied → Click "Allow" when prompted

---

## ✅ Quick Checklist

- [ ] Extension installed
- [ ] Google Cloud project created
- [ ] Google Sheets API enabled
- [ ] OAuth credentials configured
- [ ] Client ID in manifest.json
- [ ] Extension reloaded
- [ ] F9 tested successfully
- [ ] URL appears in sheet

---

## 🎯 Next Steps

After installation:
1. ✅ Test with multiple URLs
2. ✅ Customize Google Sheet URL
3. ✅ Share sheet with team (optional)
4. ✅ Add custom columns (optional)
5. ✅ Set up filters in sheet (optional)

---

## 📈 Version Information

- **Version:** 1.0.0
- **Manifest Version:** 3
- **Created:** 2026-02-14
- **Last Updated:** 2026-02-14
- **Compatibility:** Chrome 88+, Edge 88+

---

## 🎉 Success Criteria

You'll know it's working when:
- ✅ F9 shows notification
- ✅ URL appears in Google Sheet
- ✅ Number auto-increments
- ✅ Status has dropdown
- ✅ Cells are centered
- ✅ Status is red by default

---

## 📝 License

MIT License - Free to use and modify

---

**Ready to start?** → Open [QUICKSTART.md](QUICKSTART.md) now!

**Need help?** → Check [INSTALL.md](INSTALL.md) for detailed instructions.

**Want details?** → Read [SUMMARY.md](SUMMARY.md) for technical info.

