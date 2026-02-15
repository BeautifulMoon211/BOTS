# 🔧 Troubleshooting Guide

## Common Errors and Solutions

---

### ❌ Error: `ERR_FILE_NOT_FOUND` for `userReportLinkedCandidate.json`

**Error Message:**
```
chrome-extension://xxxxx/assets/userReportLinkedCandidate.json:1
Failed to load resource: net::ERR_FILE_NOT_FOUND
```

**What This Means:**
This error is **NOT from LinkCopier**. It's from another Chrome extension you have installed (likely a LinkedIn-related extension or job search tool).

**Solution:**
1. **Ignore it** - This doesn't affect LinkCopier at all
2. **Or disable other extensions** to find which one is causing it:
   - Go to `chrome://extensions/`
   - Disable extensions one by one
   - Reload the page to see which one causes the error
   - Update or remove that extension

**LinkCopier doesn't use any JSON files**, so this error is definitely from another extension.

---

### ❌ Error: Row Level Security Policy Violation

**Error Message:**
```
✗ Error: Failed to add URL: - {"code":"42501","message":"new row violates row-level security policy for table \"urls\""}
```

**Solution:**
Disable Row Level Security in Supabase:

1. Go to [supabase.com/dashboard](https://supabase.com/dashboard)
2. Select your project
3. Click **Table Editor** → **urls** table
4. Click the **RLS badge** at the top
5. Click **"Disable RLS"**
6. ✅ Try again!

**Or via SQL:**
```sql
ALTER TABLE urls DISABLE ROW LEVEL SECURITY;
```

---

### ❌ Error: "Supabase not configured"

**Error Message:**
```
✗ Error: Supabase not configured. Please set up in Settings.
```

**Solution:**
1. Click extension icon → **Settings**
2. Enter your Supabase credentials:
   - **Project URL:** `https://xxxxx.supabase.co`
   - **API Key:** Your anon public key
3. Click **"Save Settings"**
4. Click **"Test Connection"**
5. ✅ Should see success message

**Get credentials:**
- Supabase Dashboard → Settings → API
- Copy Project URL and anon public key

---

### ❌ Error: "Connection failed"

**Possible Causes:**

**1. Wrong API Key**
- Make sure you copied the **anon public** key (not service_role)
- The key should start with `eyJhbGci...`
- Copy the entire key (it's very long!)

**2. Wrong URL**
- URL should be: `https://xxxxx.supabase.co`
- No trailing slash
- Must start with `https://`

**3. Table doesn't exist**
- Go to Supabase → Table Editor
- Make sure `urls` table exists
- Create it if missing (see SUPABASE-SETUP.md)

**4. Row Level Security enabled**
- See solution above

---

### ❌ F9 Doesn't Work

**Solutions:**

**1. Set F9 manually:**
- Go to `chrome://extensions/shortcuts`
- Find LinkCopier
- Click pencil icon
- Press F9
- See F9-SETUP.md for details

**2. Use default shortcut:**
- Windows/Linux: `Ctrl+Shift+U`
- Mac: `Command+Shift+U`

**3. Check for conflicts:**
- Another extension might be using F9
- Disable other extensions temporarily
- Or use a different shortcut

---

### ❌ No Visual Feedback When Pressing F9

**Expected Behavior:**
When you press F9, you should see:
1. ✅ **On-page notification** (top-right corner, green box)
2. ✅ **System notification** (from Chrome)
3. ✅ **Badge on extension icon** (✓ or ✗)

**If you don't see these:**

**1. Check if URL was saved:**
- Go to Supabase dashboard
- Check Table Editor → urls table
- See if the URL is there

**2. Check browser notifications:**
- Make sure Chrome notifications are enabled
- Settings → Privacy and security → Site Settings → Notifications
- Allow notifications for Chrome

**3. Reload extension:**
- Go to `chrome://extensions/`
- Find LinkCopier
- Click reload icon (🔄)

---

### ❌ Extension Won't Load

**Error: "Manifest file is missing or unreadable"**
- Make sure you selected the correct folder
- The folder should contain `manifest.json`

**Error: "Invalid manifest"**
- The manifest.json has been fixed
- Make sure you have the latest version
- Try reloading the extension

**Error: "Developer mode is required"**
- Go to `chrome://extensions/`
- Enable "Developer mode" toggle (top-right)

---

### ❌ "Failed to add URL" (Generic)

**Check these in order:**

1. **Is Supabase configured?**
   - Extension Settings → Check URL and API key

2. **Test connection:**
   - Extension Settings → Click "Test Connection"
   - Should see success message

3. **Check RLS:**
   - Supabase → Table Editor → urls
   - Make sure RLS is disabled

4. **Check table structure:**
   - Table should have columns: `number`, `url`, `status`
   - All columns should be correct type (see SUPABASE-SETUP.md)

5. **Check Supabase project:**
   - Make sure project is active (not paused)
   - Free tier projects pause after 1 week of inactivity
   - Go to dashboard to wake it up

---

### ❌ URLs Saved But Not Visible in Dashboard

**Solution:**
1. Refresh the Supabase Table Editor page
2. Click on the `urls` table again
3. Check filters (make sure no filters are applied)
4. Check you're looking at the right project

---

### ❌ Duplicate Numbers in Database

**Cause:**
Multiple tabs/windows pressing F9 at the same time

**Solution:**
This is rare but can happen. The extension gets the next number, but if two requests happen simultaneously, they might get the same number.

**Fix:**
Run this SQL in Supabase to renumber:
```sql
WITH numbered AS (
  SELECT id, ROW_NUMBER() OVER (ORDER BY created_at) as new_number
  FROM urls
)
UPDATE urls
SET number = numbered.new_number
FROM numbered
WHERE urls.id = numbered.id;
```

---

### ❌ Extension Icon Shows ✗ Badge

**Meaning:**
The last F9 press failed to save the URL

**Check:**
1. Click the extension icon to see the error message
2. Follow the error-specific solution above
3. Most common: RLS is enabled or Supabase not configured

---

## 🆘 Still Having Issues?

### Check Console Logs
1. Right-click extension icon → **Inspect popup**
2. Go to **Console** tab
3. Look for error messages
4. Check what the actual error is

### Check Background Script Logs
1. Go to `chrome://extensions/`
2. Find LinkCopier
3. Click **"service worker"** link
4. Check Console for errors

### Verify Files
Make sure these files exist:
- `manifest.json`
- `background.js`
- `popup.html`
- `popup.js`
- `options.html`
- `options.js`
- `icon.png`

### Reset Extension
1. Remove extension from Chrome
2. Reload the folder
3. Reconfigure Supabase settings
4. Test again

---

## 📚 More Help

- **SUPABASE-SETUP.md** - Complete setup guide
- **F9-SETUP.md** - Keyboard shortcut setup
- **START-HERE.md** - Quick start guide
- **README.md** - Full documentation

---

**Still stuck?** Check the error message carefully and match it to the solutions above!

