# LCCopier Watchdog Monitor

## Overview
The watchdog monitor automatically keeps `LCCopier.exe` running and logs any crashes or errors.

## Features
- ✅ Automatically starts `LCCopier.exe` if not running
- ✅ Restarts the process if it crashes
- ✅ Logs all crashes with detailed error information
- ✅ Prevents infinite restart loops (max 5 restarts per minute)
- ✅ Saves logs in the same directory as the executable

## Log Files
The watchdog creates two log files in the same directory as `LCCopier.exe`:

1. **`LCCopier_watchdog.log`** - General monitoring log
   - Process start/stop events
   - Watchdog status messages
   - Timestamps for all events

2. **`LCCopier_crashes.log`** - Detailed crash information
   - Exit codes
   - Error messages
   - Stack traces
   - STDOUT/STDERR output

## Usage

### Option 1: Run as Python Script
```bash
python watchdog.py
```

### Option 2: Build and Run as Executable
```bash
# Build the watchdog
pyinstaller watchdog.spec

# Run the watchdog
dist\LCCopier_Watchdog\LCCopier_Watchdog.exe
```

## How It Works

1. **Monitoring**: Checks every 2 seconds if `LCCopier.exe` is running
2. **Auto-Start**: If not running, starts the process automatically
3. **Crash Detection**: If the process ends unexpectedly, logs the error
4. **Auto-Restart**: Waits 1 second, then restarts the process
5. **Safety Limit**: Stops after 5 restart attempts within 60 seconds

## Configuration

You can modify these settings in `watchdog.py`:

```python
PROCESS_NAME = "LCCopier.exe"        # Process to monitor
CHECK_INTERVAL = 2                    # Check every 2 seconds
RESTART_DELAY = 1                     # Wait 1 second before restart
MAX_RESTART_ATTEMPTS = 5              # Max restarts in time window
RESTART_WINDOW = 60                   # Time window in seconds
```

## Stopping the Watchdog

Press `Ctrl+C` in the console window to stop the watchdog.

## Deployment

For production use:
1. Build both executables:
   ```bash
   pyinstaller LiveCaption.spec
   pyinstaller watchdog.spec
   ```

2. Copy `LCCopier.exe` to the watchdog directory:
   ```bash
   copy dist\LCCopier\LCCopier.exe dist\LCCopier_Watchdog\
   ```

3. Run the watchdog:
   ```bash
   dist\LCCopier_Watchdog\LCCopier_Watchdog.exe
   ```

## Troubleshooting

### Watchdog can't find LCCopier.exe
- Make sure `LCCopier.exe` is in the same directory as the watchdog
- Check the log file for the exact path being searched

### Too many restarts error
- Check `LCCopier_crashes.log` for the actual error
- The application may have a critical bug preventing it from running
- Fix the underlying issue before restarting the watchdog

### No logs being created
- Check write permissions in the directory
- Run the watchdog as administrator if needed

## Example Log Output

**LCCopier_watchdog.log:**
```
[2024-01-15 10:30:00] Watchdog started. Monitoring: LCCopier.exe
[2024-01-15 10:30:00] LCCopier.exe is not running
[2024-01-15 10:30:00] Starting LCCopier.exe...
[2024-01-15 10:30:01] Successfully started LCCopier.exe (PID: 12345)
[2024-01-15 10:35:22] Process ended with return code: -1
[2024-01-15 10:35:22] Crash details written to: LCCopier_crashes.log
[2024-01-15 10:35:23] Starting LCCopier.exe...
```

**LCCopier_crashes.log:**
```
================================================================================
CRASH DETECTED: 2024-01-15 10:35:22
================================================================================
Process terminated unexpectedly
Return code: -1

STDERR:
Traceback (most recent call last):
  File "main.py", line 245, in _poll_captions
    ...
================================================================================
```

