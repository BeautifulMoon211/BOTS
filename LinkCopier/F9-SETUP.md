# Setting Up F9 Keyboard Shortcut

## Why F9 is Not Pre-configured

Chrome extensions have a limitation: **function keys (F1-F12) cannot be set in the manifest.json file**. 

The manifest only allows these formats:
- ✅ `Ctrl+Shift+[A-Z]`
- ✅ `Alt+Shift+[A-Z]`
- ✅ `Command+Shift+[A-Z]` (Mac)
- ❌ `F1` through `F12` (NOT allowed in manifest)

However, you **CAN** set F9 manually after installing the extension!

---

## How to Set F9 as Your Shortcut

### Step 1: Load the Extension First

1. Open `chrome://extensions/`
2. Enable "Developer mode"
3. Click "Load unpacked"
4. Select the LinkCopier folder
5. Extension loads successfully ✅

### Step 2: Open Keyboard Shortcuts

1. Navigate to: `chrome://extensions/shortcuts`
   - Or click the menu (☰) in the extensions page
   - Select "Keyboard shortcuts"

### Step 3: Set F9

1. Find **"LinkCopier - Real-time URL to Google Sheets"**
2. Look for the command: **"Copy current URL to Google Sheet"**
3. You'll see the current shortcut: `Ctrl+Shift+U`
4. Click the **pencil icon (✏️)** or click in the input field
5. Press **F9** on your keyboard
6. The field should now show: `F9`
7. Click outside the input or press Enter to save
8. ✅ **F9 is now set!**

---

## Visual Guide

```
chrome://extensions/shortcuts
┌─────────────────────────────────────────────────────────┐
│ LinkCopier - Real-time URL to Google Sheets            │
│                                                         │
│ Copy current URL to Google Sheet                       │
│ ┌──────────────────┐  ✏️                               │
│ │ Ctrl+Shift+U     │  ← Click pencil to edit          │
│ └──────────────────┘                                    │
│                                                         │
│ After clicking pencil:                                  │
│ ┌──────────────────┐                                    │
│ │ Type shortcut... │  ← Press F9 here                  │
│ └──────────────────┘                                    │
│                                                         │
│ After pressing F9:                                      │
│ ┌──────────────────┐                                    │
│ │ F9               │  ✅ Saved!                         │
│ └──────────────────┘                                    │
└─────────────────────────────────────────────────────────┘
```

---

## Testing F9

After setting F9:

1. Navigate to any webpage (e.g., https://google.com)
2. Press **F9** on your keyboard
3. You should see a notification: "URL #1 added to sheet successfully!"
4. Check your Google Sheet - the URL should be there!

---

## Default Shortcut (If You Don't Want F9)

If you prefer not to change it, the default shortcut is:
- **Windows/Linux:** `Ctrl+Shift+U`
- **Mac:** `Command+Shift+U`

This works immediately after installation without any configuration.

---

## Troubleshooting

### F9 Doesn't Work

**Check these:**
1. ✅ Did you set F9 in `chrome://extensions/shortcuts`?
2. ✅ Is the extension enabled?
3. ✅ Are you on a regular webpage (not chrome:// pages)?
4. ✅ Does F9 conflict with another extension?

### F9 is Already Used

If F9 is already assigned to another extension:
1. Go to `chrome://extensions/shortcuts`
2. Find the other extension using F9
3. Change or remove that shortcut first
4. Then assign F9 to LinkCopier

### Can't Set F9

Some browsers or systems may have F9 reserved:
- **Solution 1:** Use the default `Ctrl+Shift+U`
- **Solution 2:** Try `Ctrl+Shift+L` (for "Link")
- **Solution 3:** Try `Alt+Shift+U`

---

## Alternative: Use the Popup

If keyboard shortcuts don't work for you:
1. Click the LinkCopier extension icon
2. Click "Copy Current URL Now" button
3. Same result as pressing F9!

---

## Why This Limitation Exists

Chrome restricts function keys in manifests because:
- Function keys are often used by the browser itself
- They can conflict with system shortcuts
- They vary across different operating systems
- Manual assignment gives users more control

This is a Chrome/Chromium limitation, not a bug in LinkCopier.

---

## Summary

| Step | Action |
|------|--------|
| 1 | Load extension in Chrome |
| 2 | Go to `chrome://extensions/shortcuts` |
| 3 | Find LinkCopier |
| 4 | Click pencil icon next to the command |
| 5 | Press F9 |
| 6 | ✅ Done! |

---

**After following these steps, F9 will work exactly as intended!** 🎉

For more help, see:
- **QUICKSTART.md** - Quick setup guide
- **INSTALL.md** - Detailed installation
- **README.md** - Complete documentation

