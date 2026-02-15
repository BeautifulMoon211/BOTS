# 🚀 Supabase Setup Guide - 100% Free!

## ✨ Why Supabase?

✅ **Completely FREE** - No credit card required  
✅ **No OAuth complexity** - Simple API key authentication  
✅ **500MB database** - Store thousands of URLs  
✅ **Unlimited API requests** - No daily limits  
✅ **Web dashboard** - View and manage your data  
✅ **Real-time updates** - See changes instantly  

**Google Sheets API requires pre-payment. Supabase is 100% free forever!**

---

## 📋 Step-by-Step Setup (5 minutes)

### Step 1: Create Supabase Account (2 minutes)

1. Go to [supabase.com](https://supabase.com)
2. Click **"Start your project"**
3. Sign up with:
   - GitHub account (recommended)
   - Google account
   - Email/password
4. ✅ Account created!

---

### Step 2: Create New Project (2 minutes)

1. Click **"New project"**
2. Fill in details:
   - **Name:** `LinkCopier` (or any name you like)
   - **Database Password:** Create a strong password (save it!)
   - **Region:** Choose closest to your location
   - **Pricing Plan:** Free (default)
3. Click **"Create new project"**
4. ⏳ Wait 2 minutes for project setup
5. ✅ Project ready!

---

### Step 3: Create Database Table (1 minute)

1. In your project dashboard, click **"Table Editor"** (left sidebar)
2. Click **"Create a new table"**
3. Configure table:
   - **Name:** `urls`
   - **Description:** "Stores URLs from LinkCopier extension"
   - **Enable Row Level Security (RLS):** ❌ **Turn OFF** (important!)
4. Add columns (click "+ Add column"):

| Column Name | Type | Default Value | Primary | Required |
|-------------|------|---------------|---------|----------|
| `id` | int8 | Auto-increment | ✅ Yes | ✅ Yes |
| `created_at` | timestamptz | now() | ❌ No | ✅ Yes |
| `number` | int4 | (none) | ❌ No | ✅ Yes |
| `url` | text | (none) | ❌ No | ✅ Yes |
| `status` | text | 'No' | ❌ No | ✅ Yes |
| `copied` | text | 'No' | ❌ No | ✅ Yes |

5. Click **"Save"**
6. ✅ Table created!

**Important:** Make sure **Row Level Security (RLS) is DISABLED** or your extension won't be able to write data!

---

### Step 4: Get API Credentials (30 seconds)

1. Click **"Settings"** (⚙️ icon in left sidebar)
2. Click **"API"**
3. You'll see:
   - **Project URL:** `https://xxxxx.supabase.co`
   - **API Keys:**
     - `anon` `public` key (this is what you need!)
     - `service_role` `secret` key (don't use this)

4. **Copy these two values:**
   - ✅ Project URL
   - ✅ anon public key (the long one starting with `eyJhbGci...`)

---

### Step 5: Configure Extension (30 seconds)

1. Open Chrome and go to your LinkCopier extension
2. Click the extension icon
3. Click **"⚙️ Settings"**
4. Paste your credentials:
   - **Supabase Project URL:** `https://xxxxx.supabase.co`
   - **Supabase API Key:** `eyJhbGci...` (the long key)
5. Click **"💾 Save Settings"**
6. Click **"🔌 Test Connection"**
7. You should see: **"✓ Connected successfully! Database has 0 URLs stored."**
8. ✅ Setup complete!

---

## 🎯 Test It!

1. Navigate to any webpage (e.g., https://google.com)
2. Press **F9** (or Ctrl+Shift+U)
3. You should see notification: **"✓ URL #1 saved to database!"**
4. Go back to Supabase → Table Editor → `urls` table
5. ✅ Your URL is there!

---

## 📊 View Your Data

### Option 1: Supabase Dashboard
1. Go to [supabase.com/dashboard](https://supabase.com/dashboard)
2. Select your project
3. Click **"Table Editor"**
4. Click **"urls"** table
5. See all your saved URLs!

### Option 2: Extension Settings
1. Click LinkCopier extension icon
2. Click **"⚙️ Settings"**
3. Click **"📊 View Database"**
4. Opens Supabase dashboard directly!

---

## 🔧 Database Structure

Your `urls` table will look like this:

| id | created_at | number | url | status | copied |
|----|------------|--------|-----|--------|--------|
| 1 | 2026-02-15 10:30:00 | 1 | https://google.com | No | No |
| 2 | 2026-02-15 10:31:15 | 2 | https://github.com | No | Yes |
| 3 | 2026-02-15 10:32:45 | 3 | https://reddit.com | Yes | No |

- **id:** Auto-increment primary key
- **created_at:** Timestamp when URL was added
- **number:** Sequential number (1, 2, 3...)
- **url:** The webpage URL
- **status:** "Yes" or "No" (you can edit in dashboard)
- **copied:** "No" or "Yes" (tracks if exported to Google Sheets)

---

## 🆘 Troubleshooting

### "Connection failed" error

**Problem:** Row Level Security (RLS) is enabled  
**Solution:**
1. Go to Supabase → Table Editor
2. Click on `urls` table
3. Click **"RLS disabled"** badge to disable it
4. Try again

### "Table 'urls' does not exist"

**Problem:** Table not created  
**Solution:** Follow Step 3 above to create the table

### "Invalid API key"

**Problem:** Wrong key copied  
**Solution:**
1. Make sure you copied the **anon public** key (not service_role)
2. The key should start with `eyJhbGci...`
3. Copy the entire key (it's very long!)

### "CORS error"

**Problem:** Browser blocking request  
**Solution:** This shouldn't happen with Supabase, but if it does:
1. Check that your Project URL is correct
2. Make sure it starts with `https://`
3. Reload the extension

---

## 💡 Pro Tips

### Edit Status in Dashboard
1. Go to Supabase Table Editor
2. Click on any row
3. Change "No" to "Yes" in the status column
4. Click outside to save

### Export Your Data
1. Go to Supabase Table Editor
2. Click **"..."** menu
3. Select **"Export as CSV"**
4. Download your URLs!

### Delete Old URLs
1. Go to Supabase Table Editor
2. Select rows to delete
3. Click **"Delete"** button
4. Confirm deletion

### View Statistics
1. Go to Supabase → SQL Editor
2. Run query:
   ```sql
   SELECT 
     COUNT(*) as total_urls,
     COUNT(CASE WHEN status = 'Yes' THEN 1 END) as completed,
     COUNT(CASE WHEN status = 'No' THEN 1 END) as pending
   FROM urls;
   ```
3. See your stats!

---

## 🎉 You're Done!

Your LinkCopier extension is now using **100% free Supabase** instead of Google Sheets!

**No payment required. No OAuth complexity. Just works!** ✨

---

## 📚 Next Steps

- Press F9 on any webpage to save URLs
- View your data in Supabase dashboard
- Edit status (Yes/No) directly in the table
- Export data as CSV anytime
- Share your Supabase project with team members

---

**Questions?** Check the [Supabase Documentation](https://supabase.com/docs) or see README.md for more help.

