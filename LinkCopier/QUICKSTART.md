# LinkCopier - Quick Start Guide

## 🚀 Get Started in 5 Minutes

### What You'll Need
- Chrome or Edge browser
- Google account
- 5 minutes of your time

---

## Step 1: Load the Extension (30 seconds)

1. Open Chrome/Edge
2. Type `chrome://extensions/` in address bar
3. Toggle **Developer mode** ON (top-right)
4. Click **Load unpacked**
5. Select the `LinkCopier` folder
6. ✅ Extension loaded!

**📋 Copy your Extension ID** (you'll need it in Step 2)

---

## Step 2: Set Up Google Cloud (2 minutes)

### A. Create Project & Enable API

1. Go to [console.cloud.google.com](https://console.cloud.google.com/)
2. Create new project → Name it "LinkCopier"
3. Go to **APIs & Services** → **Library**
4. Search "Google Sheets API" → Click **Enable**

### B. Create OAuth Credentials

1. Go to **APIs & Services** → **Credentials**
2. Click **Create Credentials** → **OAuth client ID**
3. If prompted, configure consent screen:
   - User Type: **External**
   - App name: **LinkCopier**
   - Add your email
   - Add scope: `https://www.googleapis.com/auth/spreadsheets`
   - Add yourself as test user
4. Create OAuth client:
   - Type: **Chrome Extension**
   - Name: **LinkCopier**
   - Item ID: **[Paste Extension ID from Step 1]**
5. **📋 Copy the Client ID** (looks like: `xxxxx.apps.googleusercontent.com`)

---

## Step 3: Update Extension (1 minute)

1. Open `LinkCopier/manifest.json` in any text editor
2. Find this section:
   ```json
   "oauth2": {
     "client_id": "YOUR_CLIENT_ID.apps.googleusercontent.com",
   ```
3. Replace `YOUR_CLIENT_ID` with your actual Client ID
4. Save the file
5. Go back to `chrome://extensions/`
6. Click the **reload** button (🔄) on LinkCopier

---

## Step 4: Set F9 Shortcut (30 seconds)

1. Navigate to `chrome://extensions/shortcuts`
2. Find **LinkCopier**
3. Click the **pencil icon** next to "Copy current URL to Google Sheet"
4. Press **F9** on your keyboard
5. ✅ F9 is now set!

**Note:** Chrome doesn't allow F9 to be pre-configured in the manifest, so you need to set it manually once. See **F9-SETUP.md** for details.

**Alternative:** Use the default shortcut `Ctrl+Shift+U` without any setup.

---

## Step 5: Test It! (1 minute)

1. Navigate to **any webpage** (e.g., google.com)
2. Press **F9** (or `Ctrl+Shift+U`) on your keyboard
3. Click **Allow** when prompted for permissions
4. You should see: **"URL #1 added to sheet successfully!"**
5. Open the [default Google Sheet](https://docs.google.com/spreadsheets/d/1p7n7mjurMZWstJKEeQw7gMici6VOOdlScVB6Jav0HVM)
6. ✅ Your URL should be there!

---

## 🎉 You're Done!

### Now You Can:
- Press **F9** on any webpage to save its URL
- Click the extension icon for more options
- Open Settings to use your own Google Sheet

---

## 📊 What Gets Added to the Sheet?

Each time you press F9, a new row is added with:

| Number | URL | Status |
|--------|-----|--------|
| 1 | https://example.com | No ⬇️ |

- **Number**: Auto-increments (1, 2, 3...)
- **URL**: The current webpage URL
- **Status**: Dropdown with "Yes" (Green) or "No" (Red, default)
- **All cells**: Centered automatically

---

## ⚙️ Customize Your Setup

### Use Your Own Google Sheet

1. Create a new Google Sheet
2. Add headers: **Number** | **URL** | **Status**
3. Copy the sheet URL
4. Click LinkCopier icon → **⚙️ Settings**
5. Paste your sheet URL
6. Click **Save Settings**
7. Click **Test Connection** to verify

### Change the Keyboard Shortcut

1. Go to `chrome://extensions/shortcuts`
2. Find **LinkCopier**
3. Click the pencil icon
4. Press your preferred key combination
5. Done!

---

## 🆘 Troubleshooting

### "Failed to add URL"
- ✓ Check that Google Sheets API is enabled
- ✓ Verify Client ID in manifest.json is correct
- ✓ Make sure you clicked "Allow" for permissions

### No notification appears
- ✓ Check browser notification settings
- ✓ Press F12 and look for errors in console
- ✓ Make sure extension is enabled

### Extension ID changed
- ✓ Update the ID in Google Cloud Console credentials
- ✓ Reload the extension

### Still stuck?
1. Open `chrome://extensions/`
2. Find LinkCopier
3. Click **"service worker"** to see console
4. Look for error messages
5. Check **INSTALL.md** for detailed help

---

## 📚 More Information

- **README.md** - Complete documentation
- **INSTALL.md** - Detailed installation guide
- **CHECKLIST.md** - Step-by-step checklist
- **SUMMARY.md** - Technical overview
- **test.html** - Test page for the extension

---

## 💡 Pro Tips

1. **Share with team**: Share your Google Sheet with colleagues so everyone can add URLs
2. **Use filters**: Add filters to the sheet to sort/search URLs
3. **Add columns**: Add more columns for notes, tags, dates, etc.
4. **Conditional formatting**: The Status column already has colors - Yes is green, No is red
5. **Keyboard shortcut**: F9 works on ANY webpage, even YouTube, Gmail, etc.

---

## 🎯 Common Use Cases

- 📚 **Research**: Save article URLs while reading
- 🛍️ **Shopping**: Track product pages
- 💼 **Job Search**: Collect job posting links
- 📝 **Content Curation**: Gather resources for a project
- 🔗 **Link Management**: Better than bookmarks!

---

## ✅ Quick Checklist

- [ ] Extension loaded in Chrome
- [ ] Google Cloud project created
- [ ] Google Sheets API enabled
- [ ] OAuth credentials created
- [ ] Client ID added to manifest.json
- [ ] Extension reloaded
- [ ] F9 tested and working
- [ ] URL appears in Google Sheet

---

**🎊 Congratulations!** You now have a powerful URL collection tool at your fingertips!

Press **F9** anytime to save a URL. It's that simple!

---

**Need help?** Check the other documentation files or open the browser console (F12) for error messages.

