# 🔒 Duplicate URL Prevention

## Overview

LinkCopier now automatically prevents saving duplicate URLs to your database. If you try to save a URL that already exists, you'll get a friendly warning instead of creating a duplicate entry.

---

## How It Works

### Before Saving
1. **Check Database:** Extension queries Supabase for exact URL match
2. **Compare:** Checks if the URL already exists
3. **Decision:**
   - ✅ **New URL:** Saves to database with next number
   - ⚠️ **Duplicate:** Shows warning with existing number

### URL Matching
- **Exact match only:** `https://google.com` ≠ `https://google.com/`
- **Case sensitive:** `https://Google.com` ≠ `https://google.com`
- **Protocol matters:** `http://site.com` ≠ `https://site.com`

---

## User Experience

### New URL (Success)
**Press F9 on a new page:**
```
✓ URL #5 saved to database!
```
- Green notification
- Badge shows ✓
- URL added to database

### Duplicate URL (Warning)
**Press F9 on already-saved page:**
```
⚠ Already saved as #3
```
- Orange notification
- Badge shows ⚠
- No duplicate created
- Shows original entry number

### Error (Failure)
**If something goes wrong:**
```
✗ Failed: Connection error
```
- Red notification
- Badge shows ✗
- Error message displayed

---

## Notifications

### System Notification
- **Success:** "✓ LinkCopier Success - URL #5 saved to database!"
- **Duplicate:** "⚠ URL Already Exists - This URL is already saved as #3"
- **Error:** "✗ LinkCopier Error - Failed: [error message]"

### On-Page Notification
- **Success:** Green popup "✓ URL #5 saved!"
- **Duplicate:** Orange popup "⚠ Already saved as #3"
- **Error:** Red popup "✗ Failed: [error]"

### Badge
- **Success:** Green ✓ badge (3 seconds)
- **Duplicate:** Orange ⚠ badge (3 seconds)
- **Error:** Red ✗ badge (3 seconds)

---

## Technical Details

### Database Query
```javascript
// Check for exact URL match
const response = await fetch(
  `${config.url}/rest/v1/urls?url=eq.${encodeURIComponent(url)}&select=id,number`
);
```

### Response Format
```javascript
// New URL
{ success: true, number: 5 }

// Duplicate URL
{ 
  success: false, 
  error: "URL already exists as #3",
  duplicate: true,
  existingNumber: 3
}

// Error
{ success: false, error: "Connection failed" }
```

---

## Benefits

✅ **No Duplicates:** Database stays clean  
✅ **Instant Feedback:** Know immediately if URL exists  
✅ **Reference Number:** See which entry already has the URL  
✅ **No Data Loss:** Original entry preserved  
✅ **Fast Check:** Query happens before insertion  

---

## Examples

### Scenario 1: First Time Saving
1. Visit `https://github.com`
2. Press F9
3. Result: "✓ URL #1 saved to database!"

### Scenario 2: Trying to Save Again
1. Visit `https://github.com` (same URL)
2. Press F9
3. Result: "⚠ Already saved as #1"
4. Database: Still only has 1 entry

### Scenario 3: Similar But Different
1. Visit `https://github.com/user/repo`
2. Press F9
3. Result: "✓ URL #2 saved to database!"
4. Database: Now has 2 entries (different URLs)

---

## FAQ

### Q: What if I want to save the same URL twice?
**A:** The extension prevents this to keep your database clean. If you need multiple entries, you can manually add them in the Supabase dashboard.

### Q: Does it check the entire URL or just the domain?
**A:** It checks the **entire URL** including path, query parameters, and hash. `https://site.com/page1` and `https://site.com/page2` are different.

### Q: What about URLs with tracking parameters?
**A:** URLs with different parameters are treated as different:
- `https://site.com?ref=a` ≠ `https://site.com?ref=b`

### Q: Can I disable duplicate checking?
**A:** Not currently. This feature ensures data integrity. If you need duplicates, add them manually in Supabase.

### Q: How fast is the duplicate check?
**A:** Very fast! Usually under 100ms. The check happens before insertion, so there's no noticeable delay.

### Q: What if the check fails due to network error?
**A:** If the duplicate check fails, the extension will NOT save the URL to prevent potential duplicates. You'll see an error message.

---

## Database Impact

### Before (Without Duplicate Prevention)
| id | number | url | status |
|----|--------|-----|--------|
| 1 | 1 | https://google.com | No |
| 2 | 2 | https://github.com | No |
| 3 | 3 | https://google.com | No | ← Duplicate!
| 4 | 4 | https://google.com | No | ← Duplicate!

### After (With Duplicate Prevention)
| id | number | url | status |
|----|--------|-----|--------|
| 1 | 1 | https://google.com | No |
| 2 | 2 | https://github.com | No |

Clean database, no duplicates! ✨

---

## Troubleshooting

### "Already saved" but I don't see it in database
**Solution:** Refresh the Supabase Table Editor page. The entry exists but the view might be cached.

### Getting duplicate warnings for different URLs
**Solution:** Check the exact URL in the browser address bar. It might have extra parameters or trailing slashes.

### Want to update an existing URL's status
**Solution:** Go to Supabase Table Editor, find the entry by number, and edit the status column directly.

---

**Your database stays clean and organized!** 🎉

