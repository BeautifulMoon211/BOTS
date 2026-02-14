# LinkCopier Setup Checklist

Use this checklist to ensure proper installation and configuration.

## ✅ Pre-Installation

- [ ] Chrome or Edge browser installed (version 88+)
- [ ] Google account available
- [ ] Internet connection active

## ✅ Extension Installation

- [ ] Navigate to `chrome://extensions/` (or `edge://extensions/`)
- [ ] Enable "Developer mode" toggle (top-right)
- [ ] Click "Load unpacked" button
- [ ] Select the `LinkCopier` folder
- [ ] Extension appears in extensions list
- [ ] Copy the Extension ID (looks like: `abcdefghijklmnopqrstuvwxyz123456`)

## ✅ Google Cloud Setup

- [ ] Go to [Google Cloud Console](https://console.cloud.google.com/)
- [ ] Create new project (or select existing)
- [ ] Project name: "LinkCopier" (or your choice)
- [ ] Navigate to "APIs & Services" → "Library"
- [ ] Search for "Google Sheets API"
- [ ] Click "Enable" on Google Sheets API
- [ ] API status shows "Enabled"

## ✅ OAuth Configuration

- [ ] Go to "APIs & Services" → "Credentials"
- [ ] Click "Create Credentials" → "OAuth client ID"
- [ ] Configure OAuth consent screen (if prompted):
  - [ ] User Type: External (or Internal for Workspace)
  - [ ] App name: LinkCopier
  - [ ] User support email: (your email)
  - [ ] Developer contact: (your email)
  - [ ] Add scope: `https://www.googleapis.com/auth/spreadsheets`
  - [ ] Add test user: (your email)
  - [ ] Save and continue
- [ ] Create OAuth client ID:
  - [ ] Application type: Chrome Extension
  - [ ] Name: LinkCopier Extension
  - [ ] Item ID: (paste Extension ID from earlier)
  - [ ] Click "Create"
- [ ] Copy the Client ID (format: `xxxxx.apps.googleusercontent.com`)

## ✅ Update Extension Files

- [ ] Open `LinkCopier/manifest.json` in text editor
- [ ] Find the `oauth2` section
- [ ] Replace `YOUR_CLIENT_ID` with actual Client ID
- [ ] Save the file
- [ ] Go back to `chrome://extensions/`
- [ ] Click reload button (🔄) on LinkCopier extension

## ✅ Google Sheet Preparation

- [ ] Open the default sheet or create new one:
  - Default: https://docs.google.com/spreadsheets/d/1p7n7mjurMZWstJKEeQw7gMici6VOOdlScVB6Jav0HVM
- [ ] Add headers in row 1:
  - [ ] A1: "Number"
  - [ ] B1: "URL"
  - [ ] C1: "Status"
- [ ] (Optional) Format header row:
  - [ ] Bold text
  - [ ] Center alignment
  - [ ] Blue background (#4285f4)
  - [ ] White text color

## ✅ First Test

- [ ] Navigate to any webpage (e.g., https://google.com)
- [ ] Press **F9** key
- [ ] Authorization popup appears (first time only)
- [ ] Click "Allow" to grant permissions
- [ ] Notification appears: "URL #1 added to sheet successfully!"
- [ ] Open Google Sheet
- [ ] Verify URL appears in row 2 (row 1 is headers)
- [ ] Check Number column shows "1"
- [ ] Check URL column shows the webpage URL
- [ ] Check Status column shows "No" with red background
- [ ] Check Status column has dropdown (click to see Yes/No options)
- [ ] Verify all cells are centered

## ✅ Additional Tests

- [ ] Navigate to different webpage
- [ ] Press F9 again
- [ ] Verify URL #2 is added
- [ ] Number increments to "2"
- [ ] Click extension icon (puzzle piece → LinkCopier)
- [ ] Popup opens successfully
- [ ] Click "Copy Current URL Now" button
- [ ] URL is added to sheet
- [ ] Click "⚙️ Settings" button
- [ ] Options page opens
- [ ] Sheet URL is displayed
- [ ] Click "Test Connection" button
- [ ] Success message appears with sheet name

## ✅ Customization (Optional)

- [ ] Change keyboard shortcut:
  - [ ] Go to `chrome://extensions/shortcuts`
  - [ ] Find LinkCopier
  - [ ] Click pencil icon
  - [ ] Set new shortcut
- [ ] Change Google Sheet URL:
  - [ ] Open extension settings
  - [ ] Enter new sheet URL
  - [ ] Click "Save Settings"
  - [ ] Click "Test Connection"

## ✅ Troubleshooting Checks

If something doesn't work, verify:

- [ ] Extension is enabled in `chrome://extensions/`
- [ ] Google Sheets API is enabled in Cloud Console
- [ ] OAuth Client ID matches in manifest.json and Cloud Console
- [ ] Extension ID matches in Cloud Console credentials
- [ ] Google Sheet is accessible (not private/restricted)
- [ ] Browser has internet connection
- [ ] No errors in browser console (F12)
- [ ] No errors in extension service worker console

## ✅ Final Verification

- [ ] Extension icon visible in browser toolbar
- [ ] F9 key works on any webpage
- [ ] Notifications appear for success/failure
- [ ] URLs are added to correct Google Sheet
- [ ] Auto-numbering works correctly
- [ ] Status dropdown works (can change Yes/No)
- [ ] Status colors work (Red for No, Green for Yes)
- [ ] All text is centered
- [ ] Settings page works
- [ ] Test connection works

## 🎉 Success!

If all items are checked, your LinkCopier extension is fully functional!

## 📞 Need Help?

If you're stuck on any step:

1. Check the browser console (F12) for errors
2. Check extension service worker console:
   - Go to `chrome://extensions/`
   - Find LinkCopier
   - Click "service worker" link
   - Look for error messages
3. Review INSTALL.md for detailed instructions
4. Check SUMMARY.md for technical details

## 🔄 Quick Reset

If you need to start over:

1. Remove extension from `chrome://extensions/`
2. Delete OAuth credentials from Google Cloud Console
3. Start from "Extension Installation" section above

---

**Tip:** Keep this checklist handy for future installations or when helping others set up the extension!

