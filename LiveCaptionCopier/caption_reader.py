"""
Windows Live Caption Reader using UI Automation
Reads text from Windows 11 Live Captions window in real-time
"""

import ctypes
from ctypes import wintypes
import comtypes
import comtypes.client

# UI Automation constants
UIA_NamePropertyId = 30005
UIA_AutomationIdPropertyId = 30011
UIA_ControlTypePropertyId = 30003
UIA_TextControlTypeId = 50020
UIA_WindowControlTypeId = 50032

# Load UI Automation
try:
    comtypes.client.GetModule('UIAutomationCore.dll')
    from comtypes.gen.UIAutomationClient import *
except Exception as e:
    print(f"Warning: Failed to load UI Automation: {e}")
    IUIAutomation = None
    TreeScope_Descendants = None


class LiveCaptionReader:
    """Reads text from Windows Live Captions window."""

    MAX_TEXT_LENGTH = 50000  # Prevent unbounded memory growth

    def __init__(self):
        self.uia = None
        self.caption_window = None
        self.last_text = ""
        self._init_uia()
    
    def _init_uia(self):
        """Initialize UI Automation."""
        try:
            # Check if IUIAutomation was loaded successfully
            if 'IUIAutomation' not in globals() or IUIAutomation is None:
                print("UI Automation interface not available")
                self.uia = None
                return

            self.uia = comtypes.client.CreateObject(
                '{ff48dba4-60ef-4201-aa87-54103eef594e}',
                interface=IUIAutomation
            )
        except Exception as e:
            print(f"Failed to initialize UI Automation: {e}")
            self.uia = None
    
    def find_caption_window(self):
        """Find the Live Captions window."""
        if not self.uia:
            return None
        
        try:
            root = self.uia.GetRootElement()
            
            # Create condition to find Live Captions window
            # The window class name is typically "LiveCaptionsDesktopWindow"
            condition = self.uia.CreatePropertyCondition(
                UIA_NamePropertyId, "Live captions"
            )
            
            self.caption_window = root.FindFirst(TreeScope_Descendants, condition)
            
            if not self.caption_window:
                # Try alternative name
                condition = self.uia.CreatePropertyCondition(
                    UIA_NamePropertyId, "Live Captions"
                )
                self.caption_window = root.FindFirst(TreeScope_Descendants, condition)
            
            return self.caption_window
        except Exception as e:
            print(f"Error finding caption window: {e}")
            return None
    
    def get_caption_text(self):
        """Get current caption text from Live Captions window."""
        if not self.uia:
            return ""

        try:
            # Find window if not found yet
            if not self.caption_window:
                self.find_caption_window()

            if not self.caption_window:
                return ""

            # Find all text elements in the caption window
            condition = self.uia.CreatePropertyCondition(
                UIA_ControlTypePropertyId, UIA_TextControlTypeId
            )

            text_elements = self.caption_window.FindAll(TreeScope_Descendants, condition)

            if not text_elements or text_elements.Length == 0:
                return self.last_text

            # Collect all text
            texts = []
            for i in range(text_elements.Length):
                try:
                    element = text_elements.GetElement(i)
                    if element is None:
                        continue
                    name = element.CurrentName
                    if name and isinstance(name, str):
                        texts.append(name)
                except Exception as e:
                    # Skip invalid elements
                    print(f"Warning: Failed to get element {i}: {e}")
                    continue

            if texts:
                new_text = " ".join(texts)
                # Prevent unbounded memory growth
                if len(new_text) > self.MAX_TEXT_LENGTH:
                    # Keep only the last MAX_TEXT_LENGTH characters
                    new_text = new_text[-self.MAX_TEXT_LENGTH:]
                self.last_text = new_text

            return self.last_text

        except Exception as e:
            # Window might have been closed, try to find it again
            print(f"Error getting caption text: {e}")
            self.caption_window = None
            return self.last_text
    
    def is_available(self):
        """Check if Live Captions is running."""
        try:
            return self.find_caption_window() is not None
        except Exception as e:
            print(f"Error checking caption availability: {e}")
            return False

    def __del__(self):
        """Cleanup COM objects."""
        try:
            # Release COM objects to prevent memory leaks
            self.caption_window = None
            self.uia = None
        except Exception:
            pass


# Alternative simpler approach using FindWindow
user32 = ctypes.windll.user32


def find_live_caption_hwnd():
    """Find Live Captions window handle using FindWindow."""
    # Try different window class names
    class_names = [
        "LiveCaptionsDesktopWindow",
        "Windows.UI.Core.CoreWindow",
    ]
    
    for class_name in class_names:
        hwnd = user32.FindWindowW(class_name, None)
        if hwnd:
            return hwnd
    
    # Try by title
    hwnd = user32.FindWindowW(None, "Live captions")
    if hwnd:
        return hwnd
    
    return None

