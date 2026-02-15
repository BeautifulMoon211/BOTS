# LCCopier Features

## Window Features

### ✅ Resizable Window
- **Default Size:** 500x300 pixels
- **Minimum Size:** 300x150 pixels
- **Fully Resizable:** Drag any edge or corner to resize
- Window size is remembered during the session

### ✅ Font Size Control
- **Range:** 8px to 32px
- **Default:** 13px
- **Control:** Slider with "A" icon in the toolbar
- **Live Preview:** Font size changes immediately
- **Display:** Current font size shown next to slider

## Toolbar Controls (Left to Right)

1. **Status Indicator**
   - ⏳ Waiting... (gray) - Starting up
   - 🎤 Listening... (blue) - Ready, waiting for captions
   - 🎤 Live (green) - Actively receiving captions
   - ⚠️ Win+Ctrl+L (orange) - Live Captions not running
   - ❌ Error (red) - Error occurred

2. **📋 Copy Button**
   - Copies text from marked position to end
   - If no mark, copies all text

3. **🛡️ Screen Protection**
   - OFF (gray) - Window visible in screenshots
   - ON (green) - Window hidden from screen capture

4. **📌 Taskbar Visibility**
   - Show (gray) - Visible in taskbar
   - Hide (green) - Hidden from taskbar

5. **📍 Always On Top**
   - Off (gray) - Normal window behavior
   - On (green) - Window stays on top of others

6. **A [Slider] [Size]** - Font Size Control
   - Slider: Adjust font size (8-32px)
   - Number: Shows current font size

7. **🔆 [Slider]** - Opacity Control
   - Range: 30% to 100%
   - Default: 100% (fully opaque)

## Keyboard Shortcuts

- **Ctrl+Shift+C** - Global hotkey to copy and paste marked text
  - Works even when window is not focused
  - Automatically pastes to active window

## Caption Display Features

### Click-to-Mark
- **Click** any word to mark it
- Text before mark appears in gray
- Text from mark onwards appears in green
- **Click again** on the same word to unmark

### Auto-Scroll
- Automatically scrolls to bottom when new captions arrive
- Manual scrolling is preserved when not updating

### Text Management
- **Maximum:** 10,000 characters displayed
- Automatically trims old text when limit reached
- Preserves word boundaries when trimming

## Stealth Features

When all stealth features are enabled:
- ✅ Hidden from screen capture/recording
- ✅ Hidden from taskbar
- ✅ Always visible on top
- ✅ Adjustable transparency

Perfect for:
- Recording videos without showing captions window
- Streaming while reading captions privately
- Presentations with private notes

## Technical Details

### Process Name
- **Executable:** LCCopier.exe
- **Window Title:** Caption

### Requirements
- Windows 11 Live Captions must be running
- Enable with: Win+Ctrl+L

### Logs
When run with watchdog:
- `LCCopier_watchdog.log` - General monitoring
- `LCCopier_crashes.log` - Crash details

## Tips

1. **Resize for your needs:**
   - Make it tall and narrow for side-by-side use
   - Make it wide and short for bottom overlay
   - Minimum size prevents it from becoming unusable

2. **Font size adjustment:**
   - Increase for better readability
   - Decrease to fit more text on screen
   - Changes apply immediately

3. **Opacity for overlay:**
   - Lower opacity to see through the window
   - Useful when overlaying on other content
   - Combine with "Always On Top" for persistent overlay

4. **Mark important points:**
   - Click to mark where you want to start copying
   - Everything from that point onwards will be copied
   - Great for capturing specific parts of conversation

