# 🔄 Migration from Google Sheets to Supabase

## Why We Switched

❌ **Google Sheets API** - Requires pre-payment and credit card  
✅ **Supabase** - 100% free forever, no credit card needed!

---

## What Changed

### Before (Google Sheets)
- Required Google Cloud account
- Required OAuth 2.0 setup
- Required credit card for API access
- Complex authentication flow
- Limited to 60 requests/minute

### After (Supabase)
- Free Supabase account (no credit card!)
- Simple API key authentication
- No OAuth complexity
- Unlimited requests
- 500MB database storage

---

## Quick Setup (5 minutes)

### 1. Create Supabase Account
- Go to [supabase.com](https://supabase.com)
- Sign up with GitHub/Google (free)
- Create new project

### 2. Create Database Table
- Go to Table Editor
- Create table named `urls`
- Add columns: `number`, `url`, `status`
- **Disable Row Level Security (RLS)**

### 3. Get API Credentials
- Go to Settings → API
- Copy Project URL
- Copy anon public key

### 4. Configure Extension
- Open extension settings
- Paste Supabase URL and API key
- Click "Test Connection"
- ✅ Done!

**See SUPABASE-SETUP.md for detailed step-by-step instructions.**

---

## Data Structure Comparison

### Google Sheets (Old)
| Number | URL | Status |
|--------|-----|--------|
| 1 | https://google.com | No ⬇️ |
| 2 | https://github.com | Yes ⬇️ |

### Supabase (New)
| id | created_at | number | url | status |
|----|------------|--------|-----|--------|
| 1 | 2026-02-15 10:30 | 1 | https://google.com | No |
| 2 | 2026-02-15 10:31 | 2 | https://github.com | Yes |

**Bonus:** Supabase includes timestamps and auto-increment IDs!

---

## Features Comparison

| Feature | Google Sheets | Supabase |
|---------|---------------|----------|
| **Cost** | Requires payment | 100% Free |
| **Setup Time** | 15-20 minutes | 5 minutes |
| **Authentication** | OAuth 2.0 (complex) | API Key (simple) |
| **Credit Card** | Required | Not required |
| **Storage** | Unlimited rows | 500MB (thousands of URLs) |
| **API Limits** | 60 req/min | Unlimited |
| **Web Interface** | Google Sheets | Supabase Dashboard |
| **Export Data** | CSV, Excel | CSV, JSON, SQL |
| **Real-time** | No | Yes |
| **Search** | Basic | Advanced SQL |

---

## Migrating Your Data (Optional)

If you have existing URLs in Google Sheets:

### Option 1: Manual Copy (Small datasets)
1. Open your Google Sheet
2. Copy all URLs
3. Go to Supabase Table Editor
4. Click "Insert row" for each URL
5. Paste data

### Option 2: CSV Import (Large datasets)
1. Export Google Sheet as CSV
2. Go to Supabase Table Editor
3. Click "..." menu → "Import data from CSV"
4. Upload your CSV file
5. Map columns (Number → number, URL → url, Status → status)
6. Click "Import"

### Option 3: Keep Both (Recommended)
- Keep your Google Sheet as backup
- Start fresh with Supabase
- No migration needed!

---

## Backup Files

Your old Google Sheets files are backed up as:
- `background-google.js.backup`
- `options-google.html.backup`
- `options-google.js.backup`

To restore Google Sheets version:
```bash
mv background-google.js.backup background.js
mv options-google.html.backup options.html
mv options-google.js.backup options.js
```

Then reload the extension.

---

## Troubleshooting

### "I want to go back to Google Sheets"
See backup files above. You can switch back anytime!

### "Can I use both?"
Not simultaneously, but you can switch between them by swapping the files.

### "What about my old data?"
Your Google Sheet data is safe and unchanged. This extension just stops writing to it.

---

## Benefits of Supabase

✅ **No payment** - Completely free forever  
✅ **Faster setup** - 5 minutes vs 20 minutes  
✅ **Simpler auth** - API key vs OAuth 2.0  
✅ **Better dashboard** - Modern UI with search, filters, SQL  
✅ **More features** - Real-time updates, timestamps, advanced queries  
✅ **No limits** - Unlimited API requests  
✅ **Team friendly** - Easy to share access  

---

## Next Steps

1. **Read SUPABASE-SETUP.md** - Complete setup guide
2. **Test the extension** - Press F9 on any webpage
3. **View your data** - Check Supabase dashboard
4. **Enjoy!** - No more payment worries!

---

**Questions?** See SUPABASE-SETUP.md or README.md for help.

