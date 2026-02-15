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
except:
    pass


class LiveCaptionReader:
    """Reads text from Windows Live Captions window."""
    
    def __init__(self):
        self.uia = None
        self.caption_window = None
        self.last_text = ""
        self._init_uia()
    
    def _init_uia(self):
        """Initialize UI Automation."""
        try:
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
            
            if not text_elements:
                return self.last_text
            
            # Collect all text
            texts = []
            for i in range(text_elements.Length):
                element = text_elements.GetElement(i)
                name = element.CurrentName
                if name:
                    texts.append(name)
            
            if texts:
                self.last_text = " ".join(texts)
            
            return self.last_text
            
        except Exception as e:
            # Window might have been closed, try to find it again
            self.caption_window = None
            return self.last_text
    
    def is_available(self):
        """Check if Live Captions is running."""
        return self.find_caption_window() is not None


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

