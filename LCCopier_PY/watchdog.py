"""
LCCopier Watchdog - Monitors and restarts LCCopier.exe if it crashes
Logs all errors and crash information to the same directory
"""

import os
import sys
import time
import subprocess
import psutil
from datetime import datetime
import traceback

# Configuration
PROCESS_NAME = "LCCopier.exe"
CHECK_INTERVAL = 2  # Check every 2 seconds
RESTART_DELAY = 1   # Wait 1 second before restarting
MAX_RESTART_ATTEMPTS = 5  # Max consecutive restart attempts
RESTART_WINDOW = 60  # Time window in seconds to count restarts

class LCCopierWatchdog:
    def __init__(self):
        # Get the directory where this script is located
        if getattr(sys, 'frozen', False):
            # Running as compiled executable
            self.base_dir = os.path.dirname(sys.executable)
        else:
            # Running as script
            self.base_dir = os.path.dirname(os.path.abspath(__file__))
        
        self.exe_path = os.path.join(self.base_dir, PROCESS_NAME)
        self.log_file = os.path.join(self.base_dir, "LCCopier_watchdog.log")
        self.crash_log_file = os.path.join(self.base_dir, "LCCopier_crashes.log")
        
        self.process = None
        self.restart_times = []
        
        self.log(f"Watchdog started. Monitoring: {PROCESS_NAME}")
        self.log(f"Executable path: {self.exe_path}")
        self.log(f"Log file: {self.log_file}")
    
    def log(self, message):
        """Log message to file and console."""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        log_message = f"[{timestamp}] {message}"
        print(log_message)
        
        try:
            with open(self.log_file, 'a', encoding='utf-8') as f:
                f.write(log_message + '\n')
        except Exception as e:
            print(f"Failed to write to log file: {e}")
    
    def log_crash(self, error_info):
        """Log crash information to separate crash log."""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        separator = "=" * 80
        
        crash_message = f"\n{separator}\n"
        crash_message += f"CRASH DETECTED: {timestamp}\n"
        crash_message += f"{separator}\n"
        crash_message += f"{error_info}\n"
        crash_message += f"{separator}\n\n"
        
        try:
            with open(self.crash_log_file, 'a', encoding='utf-8') as f:
                f.write(crash_message)
            self.log(f"Crash details written to: {self.crash_log_file}")
        except Exception as e:
            self.log(f"Failed to write crash log: {e}")
    
    def is_process_running(self):
        """Check if LCCopier.exe is running."""
        try:
            for proc in psutil.process_iter(['name', 'pid']):
                if proc.info['name'] == PROCESS_NAME:
                    return True, proc.info['pid']
            return False, None
        except Exception as e:
            self.log(f"Error checking process: {e}")
            return False, None
    
    def start_process(self):
        """Start LCCopier.exe."""
        try:
            if not os.path.exists(self.exe_path):
                self.log(f"ERROR: Executable not found at {self.exe_path}")
                return False
            
            self.log(f"Starting {PROCESS_NAME}...")
            
            # Start the process without console window
            startupinfo = subprocess.STARTUPINFO()
            startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            startupinfo.wShowWindow = subprocess.SW_HIDE
            
            self.process = subprocess.Popen(
                [self.exe_path],
                startupinfo=startupinfo,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                cwd=self.base_dir
            )
            
            # Wait a moment to check if it started successfully
            time.sleep(0.5)
            
            if self.process.poll() is None:
                self.log(f"Successfully started {PROCESS_NAME} (PID: {self.process.pid})")
                return True
            else:
                # Process ended immediately
                stdout, stderr = self.process.communicate()
                error_info = f"Process ended immediately after start\n"
                error_info += f"Return code: {self.process.returncode}\n"
                if stdout:
                    error_info += f"STDOUT:\n{stdout.decode('utf-8', errors='ignore')}\n"
                if stderr:
                    error_info += f"STDERR:\n{stderr.decode('utf-8', errors='ignore')}\n"
                
                self.log(f"ERROR: {PROCESS_NAME} failed to start")
                self.log_crash(error_info)
                return False
                
        except Exception as e:
            error_info = f"Exception while starting process:\n{traceback.format_exc()}"
            self.log(f"ERROR: Failed to start {PROCESS_NAME}: {e}")
            self.log_crash(error_info)
            return False
    
    def check_restart_limit(self):
        """Check if we've exceeded restart attempts."""
        current_time = time.time()
        
        # Remove old restart times outside the window
        self.restart_times = [t for t in self.restart_times if current_time - t < RESTART_WINDOW]
        
        if len(self.restart_times) >= MAX_RESTART_ATTEMPTS:
            self.log(f"ERROR: Too many restarts ({MAX_RESTART_ATTEMPTS}) within {RESTART_WINDOW} seconds")
            self.log("The application may be crashing repeatedly. Please check the crash log.")
            return False
        
        return True

    def monitor_process(self):
        """Monitor the process and capture exit information."""
        if self.process is None:
            return

        try:
            # Wait for process to end
            return_code = self.process.wait()

            # Process ended, capture output
            stdout, stderr = self.process.communicate(timeout=1)

            error_info = f"Process terminated unexpectedly\n"
            error_info += f"Return code: {return_code}\n"

            if stdout:
                error_info += f"\nSTDOUT:\n{stdout.decode('utf-8', errors='ignore')}\n"
            if stderr:
                error_info += f"\nSTDERR:\n{stderr.decode('utf-8', errors='ignore')}\n"

            self.log(f"Process ended with return code: {return_code}")
            self.log_crash(error_info)

        except subprocess.TimeoutExpired:
            pass
        except Exception as e:
            self.log(f"Error monitoring process: {e}")

    def run(self):
        """Main watchdog loop."""
        self.log("Watchdog monitoring started")

        try:
            while True:
                running, pid = self.is_process_running()

                if not running:
                    self.log(f"{PROCESS_NAME} is not running")

                    # Check if we can restart
                    if not self.check_restart_limit():
                        self.log("Stopping watchdog due to restart limit")
                        break

                    # Record restart time
                    self.restart_times.append(time.time())

                    # Wait before restarting
                    if len(self.restart_times) > 1:
                        self.log(f"Waiting {RESTART_DELAY} seconds before restart...")
                        time.sleep(RESTART_DELAY)

                    # Start the process
                    if not self.start_process():
                        self.log("Failed to start process, retrying in 5 seconds...")
                        time.sleep(5)
                else:
                    # Process is running
                    if self.process is None or self.process.pid != pid:
                        self.log(f"{PROCESS_NAME} is running (PID: {pid})")

                # Wait before next check
                time.sleep(CHECK_INTERVAL)

        except KeyboardInterrupt:
            self.log("Watchdog stopped by user (Ctrl+C)")
        except Exception as e:
            error_info = f"Watchdog error:\n{traceback.format_exc()}"
            self.log(f"ERROR: Watchdog encountered an error: {e}")
            self.log_crash(error_info)
        finally:
            self.log("Watchdog monitoring stopped")


def main():
    """Main entry point."""
    print("=" * 60)
    print("LCCopier Watchdog Monitor")
    print("=" * 60)
    print("This will monitor and restart LCCopier.exe if it crashes")
    print("Press Ctrl+C to stop the watchdog")
    print("=" * 60)
    print()

    watchdog = LCCopierWatchdog()
    watchdog.run()


if __name__ == "__main__":
    main()

