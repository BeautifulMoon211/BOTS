# LinkCopier Installation Guide

## Quick Start (Without OAuth - Limited Functionality)

If you want to test the extension quickly without setting up OAuth, you can use the Google Apps Script method below.

## Full Installation (Recommended)

### Step 1: Load the Extension

1. Open Chrome/Edge browser
2. Navigate to `chrome://extensions/` (or `edge://extensions/`)
3. Enable **Developer mode** (toggle in top-right corner)
4. Click **Load unpacked**
5. Select the `LinkCopier` folder
6. **Copy the Extension ID** (you'll need this later)

### Step 2: Set Up Google Cloud Project

1. Go to [Google Cloud Console](https://console.cloud.google.com/)
2. Create a new project:
   - Click "Select a project" → "New Project"
   - Name it "LinkCopier" or similar
   - Click "Create"

3. Enable Google Sheets API:
   - Go to "APIs & Services" → "Library"
   - Search for "Google Sheets API"
   - Click on it and press "Enable"

4. Create OAuth 2.0 Credentials:
   - Go to "APIs & Services" → "Credentials"
   - Click "Create Credentials" → "OAuth client ID"
   - If prompted, configure the OAuth consent screen:
     - User Type: External (or Internal if using Google Workspace)
     - App name: LinkCopier
     - User support email: your email
     - Developer contact: your email
     - Scopes: Add `https://www.googleapis.com/auth/spreadsheets`
     - Test users: Add your email
     - Click "Save and Continue"
   
   - Back to Create OAuth client ID:
     - Application type: **Chrome Extension**
     - Name: LinkCopier Extension
     - Item ID: Paste the Extension ID you copied earlier
     - Click "Create"
   
   - Copy the **Client ID** (looks like: `xxxxx.apps.googleusercontent.com`)

### Step 3: Update manifest.json

1. Open `LinkCopier/manifest.json`
2. Find the `oauth2` section
3. Replace `YOUR_CLIENT_ID` with your actual Client ID:
   ```json
   "oauth2": {
     "client_id": "123456789-abcdefg.apps.googleusercontent.com",
     "scopes": [
       "https://www.googleapis.com/auth/spreadsheets"
     ]
   }
   ```
4. Save the file
5. Go back to `chrome://extensions/` and click the **Reload** button on LinkCopier

### Step 4: Prepare Your Google Sheet

1. Open the default Google Sheet or create a new one:
   - Default: https://docs.google.com/spreadsheets/d/1p7n7mjurMZWstJKEeQw7gMici6VOOdlScVB6Jav0HVM
   
2. Make sure the sheet has these headers in row 1:
   - **A1**: Number
   - **B1**: URL
   - **C1**: Status

3. (Optional) Format the header row:
   - Bold text
   - Center alignment
   - Blue background (#4285f4)
   - White text

### Step 5: Test the Extension

1. Navigate to any webpage (e.g., https://google.com)
2. Press **F9** on your keyboard
3. You'll be prompted to authorize the extension (first time only)
4. Click "Allow" to grant permissions
5. You should see a notification: "URL #1 added to sheet successfully!"
6. Check your Google Sheet - the URL should be there!

## Alternative: Using Google Apps Script (No OAuth Required)

If you don't want to set up OAuth, you can use Google Apps Script as a backend:

1. Open your Google Sheet
2. Go to Extensions → Apps Script
3. Delete the default code and paste this:

```javascript
function doPost(e) {
  const sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  const data = JSON.parse(e.postData.contents);
  
  // Get next number
  const lastRow = sheet.getLastRow();
  const nextNumber = lastRow > 1 ? sheet.getRange(lastRow, 1).getValue() + 1 : 1;
  
  // Add new row
  const newRow = [nextNumber, data.url, 'No'];
  sheet.appendRow(newRow);
  
  // Format the row
  const rowIndex = sheet.getLastRow();
  const range = sheet.getRange(rowIndex, 1, 1, 3);
  range.setHorizontalAlignment('center');
  range.setVerticalAlignment('middle');
  
  // Add dropdown to Status column
  const statusCell = sheet.getRange(rowIndex, 3);
  const rule = SpreadsheetApp.newDataValidation()
    .requireValueInList(['Yes', 'No'], true)
    .build();
  statusCell.setDataValidation(rule);
  
  // Set background color for "No"
  statusCell.setBackground('#f5c6cb');
  statusCell.setFontColor('#721c24');
  statusCell.setFontWeight('bold');
  
  return ContentService.createTextOutput(JSON.stringify({
    success: true,
    number: nextNumber
  })).setMimeType(ContentService.MimeType.JSON);
}
```

4. Click "Deploy" → "New deployment"
5. Type: Web app
6. Execute as: Me
7. Who has access: Anyone
8. Click "Deploy"
9. Copy the Web App URL
10. Modify `background.js` to use this URL instead of Google Sheets API

## Troubleshooting

### Extension ID Changed
If you reload the extension and the ID changes, you need to update it in Google Cloud Console credentials.

### OAuth Error
- Make sure the Client ID in manifest.json matches the one in Google Cloud Console
- Verify the Extension ID in Google Cloud Console matches your current extension ID
- Try removing and re-adding the extension

### "Failed to add URL"
- Check browser console (F12) for detailed errors
- Verify Google Sheets API is enabled
- Make sure you've authorized the extension
- Check that the sheet URL is correct

### No Notification Appears
- Check if notifications are enabled for Chrome
- Look for errors in the browser console (F12)

## Changing the Keyboard Shortcut

1. Go to `chrome://extensions/shortcuts`
2. Find "LinkCopier"
3. Click the pencil icon next to "Copy current URL to Google Sheet"
4. Press your desired key combination
5. Click "OK"

## Support

For issues, check:
1. Browser console (F12) for errors
2. Extension background page console (chrome://extensions/ → LinkCopier → "service worker")
3. Google Cloud Console logs

## Next Steps

- Customize the Google Sheet URL in extension settings
- Set up conditional formatting rules in your sheet
- Share the sheet with team members for collaborative URL tracking

