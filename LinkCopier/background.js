// Default Google Sheet URL
const DEFAULT_SHEET_URL = 'https://docs.google.com/spreadsheets/d/1p7n7mjurMZWstJKEeQw7gMici6VOOdlScVB6Jav0HVM';

// Extract spreadsheet ID from URL
function extractSpreadsheetId(url) {
  const match = url.match(/\/spreadsheets\/d\/([a-zA-Z0-9-_]+)/);
  return match ? match[1] : null;
}

// Get the configured sheet URL from storage
async function getSheetUrl() {
  const result = await chrome.storage.sync.get(['sheetUrl']);
  return result.sheetUrl || DEFAULT_SHEET_URL;
}

// Get OAuth token
async function getAuthToken() {
  return new Promise((resolve, reject) => {
    chrome.identity.getAuthToken({ interactive: true }, (token) => {
      if (chrome.runtime.lastError) {
        reject(chrome.runtime.lastError);
      } else {
        resolve(token);
      }
    });
  });
}

// Get the next row number by reading the sheet
async function getNextRowNumber(token, spreadsheetId) {
  try {
    const response = await fetch(
      `https://sheets.googleapis.com/v4/spreadsheets/${spreadsheetId}/values/Sheet1!A:A`,
      {
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        }
      }
    );

    if (!response.ok) {
      throw new Error(`Failed to read sheet: ${response.statusText}`);
    }

    const data = await response.json();
    const values = data.values || [];
    
    // Filter out header row and empty rows, get the last number
    const numbers = values
      .slice(1) // Skip header
      .map(row => parseInt(row[0]))
      .filter(num => !isNaN(num));
    
    const lastNumber = numbers.length > 0 ? Math.max(...numbers) : 0;
    return lastNumber + 1;
  } catch (error) {
    console.error('Error getting next row number:', error);
    return 1; // Default to 1 if error
  }
}

// Add URL to Google Sheet
async function addUrlToSheet(url) {
  try {
    const sheetUrl = await getSheetUrl();
    const spreadsheetId = extractSpreadsheetId(sheetUrl);
    
    if (!spreadsheetId) {
      throw new Error('Invalid Google Sheet URL');
    }

    const token = await getAuthToken();
    const nextNumber = await getNextRowNumber(token, spreadsheetId);

    // Prepare the row data: [Number, URL, Status]
    const values = [[nextNumber, url, 'No']];

    // Append the row
    const appendResponse = await fetch(
      `https://sheets.googleapis.com/v4/spreadsheets/${spreadsheetId}/values/Sheet1!A:C:append?valueInputOption=USER_ENTERED`,
      {
        method: 'POST',
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          values: values
        })
      }
    );

    if (!appendResponse.ok) {
      throw new Error(`Failed to append to sheet: ${appendResponse.statusText}`);
    }

    const result = await appendResponse.json();
    console.log('Successfully added URL to sheet:', result);

    // Format the newly added row
    await formatNewRow(token, spreadsheetId, result.updates.updatedRange);

    return { success: true, number: nextNumber };
  } catch (error) {
    console.error('Error adding URL to sheet:', error);
    return { success: false, error: error.message };
  }
}

// Format the newly added row with centered text and dropdown
async function formatNewRow(token, spreadsheetId, updatedRange) {
  try {
    // Extract row number from range (e.g., "Sheet1!A2:C2" -> 2)
    const rowMatch = updatedRange.match(/!A(\d+):C\d+/);
    if (!rowMatch) return;

    const rowIndex = parseInt(rowMatch[1]) - 1; // Convert to 0-based index

    const requests = [
      // Center align all cells in the row
      {
        repeatCell: {
          range: {
            sheetId: 0,
            startRowIndex: rowIndex,
            endRowIndex: rowIndex + 1,
            startColumnIndex: 0,
            endColumnIndex: 3
          },
          cell: {
            userEnteredFormat: {
              horizontalAlignment: 'CENTER',
              verticalAlignment: 'MIDDLE'
            }
          },
          fields: 'userEnteredFormat(horizontalAlignment,verticalAlignment)'
        }
      },
      // Add data validation (dropdown) for Status column (column C)
      {
        setDataValidation: {
          range: {
            sheetId: 0,
            startRowIndex: rowIndex,
            endRowIndex: rowIndex + 1,
            startColumnIndex: 2,
            endColumnIndex: 3
          },
          rule: {
            condition: {
              type: 'ONE_OF_LIST',
              values: [
                { userEnteredValue: 'Yes' },
                { userEnteredValue: 'No' }
              ]
            },
            showCustomUi: true,
            strict: true
          }
        }
      },
      // Apply conditional formatting for "No" (Red background)
      {
        repeatCell: {
          range: {
            sheetId: 0,
            startRowIndex: rowIndex,
            endRowIndex: rowIndex + 1,
            startColumnIndex: 2,
            endColumnIndex: 3
          },
          cell: {
            userEnteredFormat: {
              backgroundColor: {
                red: 0.96,
                green: 0.80,
                blue: 0.80
              },
              textFormat: {
                foregroundColor: {
                  red: 0.8,
                  green: 0.0,
                  blue: 0.0
                },
                bold: true
              }
            }
          },
          fields: 'userEnteredFormat(backgroundColor,textFormat)'
        }
      }
    ];

    await fetch(
      `https://sheets.googleapis.com/v4/spreadsheets/${spreadsheetId}:batchUpdate`,
      {
        method: 'POST',
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({ requests })
      }
    );
  } catch (error) {
    console.error('Error formatting row:', error);
  }
}

// Listen for F9 command
chrome.commands.onCommand.addListener(async (command) => {
  if (command === 'copy-url-to-sheet') {
    try {
      const [tab] = await chrome.tabs.query({ active: true, currentWindow: true });
      
      if (tab && tab.url) {
        const result = await addUrlToSheet(tab.url);
        
        if (result.success) {
          // Show success notification
          chrome.notifications.create({
            type: 'basic',
            iconUrl: 'icon.png',
            title: 'LinkCopier',
            message: `URL #${result.number} added to sheet successfully!`
          });
        } else {
          // Show error notification
          chrome.notifications.create({
            type: 'basic',
            iconUrl: 'icon.png',
            title: 'LinkCopier Error',
            message: `Failed to add URL: ${result.error}`
          });
        }
      }
    } catch (error) {
      console.error('Error in command handler:', error);
    }
  }
});

// Handle messages from popup
chrome.runtime.onMessage.addListener((request, sender, sendResponse) => {
  if (request.action === 'addUrl') {
    addUrlToSheet(request.url).then(result => {
      sendResponse(result);
    });
    return true; // Keep channel open for async response
  }
});

// Initialize sheet with headers if needed
chrome.runtime.onInstalled.addListener(async () => {
  console.log('LinkCopier extension installed');
});

