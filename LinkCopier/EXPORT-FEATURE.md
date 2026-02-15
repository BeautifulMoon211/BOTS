# 📋 Export to Google Sheets Feature

## Overview

The LinkCopier extension now includes a one-click export feature that copies uncopied URLs from your Supabase database to your clipboard in a format ready to paste directly into Google Sheets.

---

## How It Works

### Workflow

1. **Click Export Button** → Opens popup, clicks "📋 Export Uncopied to Clipboard"
2. **Fetch Uncopied Rows** → Queries database for all rows where `copied = 'No'`
3. **Format as TSV** → Converts data to Tab-Separated Values (Google Sheets format)
4. **Copy to Clipboard** → Data is ready to paste
5. **Mark as Copied** → Updates all exported rows to `copied = 'Yes'`
6. **Show Confirmation** → Displays count of exported rows

---

## Using the Feature

### Step 1: Export Data

1. Click the LinkCopier extension icon
2. Click **"📋 Export Uncopied to Clipboard"** button
3. Wait for confirmation: "✓ X rows copied to clipboard!"

### Step 2: Paste into Google Sheets

1. Open your Google Sheet
2. Click on cell A1 (or wherever you want to paste)
3. Press **Ctrl+V** (or Cmd+V on Mac)
4. Data appears in 3 columns: Number, URL, Status

### Step 3: Done!

- All exported rows are now marked as `copied = 'Yes'`
- Next export will only include new URLs
- No duplicates in your Google Sheet!

---

## Data Format

### TSV (Tab-Separated Values)

The export creates data in this format:
```
1	https://google.com	No
2	https://github.com	Yes
3	https://reddit.com	No
```

### Google Sheets Result

When pasted, it creates:

| A | B | C |
|---|---|---|
| 1 | https://google.com | No |
| 2 | https://github.com | Yes |
| 3 | https://reddit.com | No |

**Column A:** Number  
**Column B:** URL  
**Column C:** Status  

---

## The 'copied' Column

### Purpose

Tracks which URLs have been exported to Google Sheets to prevent duplicates.

### Values

- **'No'** (default) - Not yet exported
- **'Yes'** - Already exported to Google Sheets

### Automatic Updates

- New URLs start with `copied = 'No'`
- After export, rows are updated to `copied = 'Yes'`
- Only `copied = 'No'` rows are included in future exports

---

## Examples

### Example 1: First Export

**Database before export:**
| number | url | status | copied |
|--------|-----|--------|--------|
| 1 | https://google.com | No | No |
| 2 | https://github.com | No | No |
| 3 | https://reddit.com | Yes | No |

**Click Export → Clipboard contains:**
```
1	https://google.com	No
2	https://github.com	No
3	https://reddit.com	Yes
```

**Database after export:**
| number | url | status | copied |
|--------|-----|--------|--------|
| 1 | https://google.com | No | **Yes** |
| 2 | https://github.com | No | **Yes** |
| 3 | https://reddit.com | Yes | **Yes** |

### Example 2: Second Export (New URLs Only)

**Add more URLs, then export again:**

**Database before second export:**
| number | url | status | copied |
|--------|-----|--------|--------|
| 1 | https://google.com | No | Yes |
| 2 | https://github.com | No | Yes |
| 3 | https://reddit.com | Yes | Yes |
| 4 | https://stackoverflow.com | No | **No** |
| 5 | https://youtube.com | No | **No** |

**Click Export → Clipboard contains:**
```
4	https://stackoverflow.com	No
5	https://youtube.com	No
```

**Only new URLs exported!** ✨

---

## Benefits

✅ **One-Click Export** - No manual copying  
✅ **Perfect Format** - Ready for Google Sheets  
✅ **No Duplicates** - Tracks what's been exported  
✅ **Incremental** - Only exports new URLs  
✅ **Fast** - Instant clipboard copy  
✅ **Reliable** - Automatic status updates  

---

## Troubleshooting

### "No uncopied rows found"

**Cause:** All rows in database have `copied = 'Yes'`  
**Solution:** Add new URLs with Ctrl+Q, then export again

### Export button doesn't work

**Cause:** Supabase not configured  
**Solution:** Go to Settings and configure Supabase credentials

### Data not pasting correctly

**Cause:** Clipboard might be overwritten  
**Solution:** Click export again and paste immediately

### Want to re-export all URLs

**Solution:** 
1. Go to Supabase Table Editor
2. Select all rows
3. Edit `copied` column to 'No'
4. Click export button again

---

## Database Setup

### Adding the 'copied' Column

If you created your database before this feature, add the column:

**Option 1: Supabase Dashboard**
1. Go to Table Editor → `urls` table
2. Click "+ Add column"
3. Name: `copied`
4. Type: `text`
5. Default value: `'No'`
6. Required: Yes
7. Click Save

**Option 2: SQL Editor**
```sql
ALTER TABLE urls ADD COLUMN copied text DEFAULT 'No' NOT NULL;
```

---

## FAQ

### Q: Can I export to Excel instead?
**A:** Yes! The TSV format works in Excel too. Just paste the same way.

### Q: What if I manually edit 'copied' in Supabase?
**A:** That's fine! Set it to 'No' to include in next export, or 'Yes' to exclude.

### Q: Does export delete rows from Supabase?
**A:** No! It only marks them as copied. All data stays in Supabase.

### Q: Can I export specific rows only?
**A:** Currently exports all `copied = 'No'` rows. For custom exports, use Supabase dashboard's export feature.

### Q: What's the maximum number of rows I can export?
**A:** No limit! The extension handles any number of rows.

---

**Seamless integration between Supabase and Google Sheets!** 🎉

