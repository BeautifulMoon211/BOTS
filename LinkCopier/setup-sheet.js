// Setup script to initialize Google Sheet with proper formatting
// This can be run from the browser console or as a standalone script

const SPREADSHEET_ID = '1p7n7mjurMZWstJKEeQw7gMici6VOOdlScVB6Jav0HVM';

async function setupSheet() {
  try {
    // Get OAuth token
    const token = await new Promise((resolve, reject) => {
      chrome.identity.getAuthToken({ interactive: true }, (token) => {
        if (chrome.runtime.lastError) {
          reject(chrome.runtime.lastError);
        } else {
          resolve(token);
        }
      });
    });

    console.log('Setting up Google Sheet...');

    // Step 1: Clear existing data (optional - comment out if you want to keep data)
    // await fetch(
    //   `https://sheets.googleapis.com/v4/spreadsheets/${SPREADSHEET_ID}/values/Sheet1!A:C:clear`,
    //   {
    //     method: 'POST',
    //     headers: {
    //       'Authorization': `Bearer ${token}`,
    //       'Content-Type': 'application/json'
    //     }
    //   }
    // );

    // Step 2: Add headers
    await fetch(
      `https://sheets.googleapis.com/v4/spreadsheets/${SPREADSHEET_ID}/values/Sheet1!A1:C1?valueInputOption=USER_ENTERED`,
      {
        method: 'PUT',
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          values: [['Number', 'URL', 'Status']]
        })
      }
    );

    console.log('Headers added');

    // Step 3: Format the sheet
    const requests = [
      // Freeze header row
      {
        updateSheetProperties: {
          properties: {
            sheetId: 0,
            gridProperties: {
              frozenRowCount: 1
            }
          },
          fields: 'gridProperties.frozenRowCount'
        }
      },
      // Format header row (bold, centered, background color)
      {
        repeatCell: {
          range: {
            sheetId: 0,
            startRowIndex: 0,
            endRowIndex: 1,
            startColumnIndex: 0,
            endColumnIndex: 3
          },
          cell: {
            userEnteredFormat: {
              backgroundColor: {
                red: 0.26,
                green: 0.52,
                blue: 0.96
              },
              textFormat: {
                foregroundColor: {
                  red: 1.0,
                  green: 1.0,
                  blue: 1.0
                },
                fontSize: 11,
                bold: true
              },
              horizontalAlignment: 'CENTER',
              verticalAlignment: 'MIDDLE'
            }
          },
          fields: 'userEnteredFormat(backgroundColor,textFormat,horizontalAlignment,verticalAlignment)'
        }
      },
      // Set column widths
      {
        updateDimensionProperties: {
          range: {
            sheetId: 0,
            dimension: 'COLUMNS',
            startIndex: 0,
            endIndex: 1
          },
          properties: {
            pixelSize: 80
          },
          fields: 'pixelSize'
        }
      },
      {
        updateDimensionProperties: {
          range: {
            sheetId: 0,
            dimension: 'COLUMNS',
            startIndex: 1,
            endIndex: 2
          },
          properties: {
            pixelSize: 400
          },
          fields: 'pixelSize'
        }
      },
      {
        updateDimensionProperties: {
          range: {
            sheetId: 0,
            dimension: 'COLUMNS',
            startIndex: 2,
            endIndex: 3
          },
          properties: {
            pixelSize: 100
          },
          fields: 'pixelSize'
        }
      },
      // Add conditional formatting for Status column
      {
        addConditionalFormatRule: {
          rule: {
            ranges: [{
              sheetId: 0,
              startRowIndex: 1,
              startColumnIndex: 2,
              endColumnIndex: 3
            }],
            booleanRule: {
              condition: {
                type: 'TEXT_EQ',
                values: [{ userEnteredValue: 'Yes' }]
              },
              format: {
                backgroundColor: {
                  red: 0.72,
                  green: 0.88,
                  blue: 0.80
                },
                textFormat: {
                  foregroundColor: {
                    red: 0.0,
                    green: 0.5,
                    blue: 0.0
                  },
                  bold: true
                }
              }
            }
          },
          index: 0
        }
      },
      {
        addConditionalFormatRule: {
          rule: {
            ranges: [{
              sheetId: 0,
              startRowIndex: 1,
              startColumnIndex: 2,
              endColumnIndex: 3
            }],
            booleanRule: {
              condition: {
                type: 'TEXT_EQ',
                values: [{ userEnteredValue: 'No' }]
              },
              format: {
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
            }
          },
          index: 1
        }
      }
    ];

    const formatResponse = await fetch(
      `https://sheets.googleapis.com/v4/spreadsheets/${SPREADSHEET_ID}:batchUpdate`,
      {
        method: 'POST',
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({ requests })
      }
    );

    if (formatResponse.ok) {
      console.log('✓ Sheet setup complete!');
      console.log('✓ Headers added and formatted');
      console.log('✓ Conditional formatting applied');
      console.log('✓ Column widths set');
      return true;
    } else {
      const error = await formatResponse.text();
      console.error('Error formatting sheet:', error);
      return false;
    }
  } catch (error) {
    console.error('Error setting up sheet:', error);
    return false;
  }
}

// Run if in extension context
if (typeof chrome !== 'undefined' && chrome.identity) {
  setupSheet();
}

