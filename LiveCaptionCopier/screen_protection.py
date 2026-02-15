"""
Windows Screen Capture Protection Module
"""

import ctypes
from ctypes import wintypes

WDA_NONE = 0x00000000
WDA_EXCLUDEFROMCAPTURE = 0x00000011

user32 = ctypes.windll.user32
SetWindowDisplayAffinity = user32.SetWindowDisplayAffinity
SetWindowDisplayAffinity.argtypes = [wintypes.HWND, wintypes.DWORD]
SetWindowDisplayAffinity.restype = wintypes.BOOL


def enable_screen_protection(hwnd: int) -> bool:
    try:
        if not hwnd:
            return False
        result = SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE)
        if not result:
            result = SetWindowDisplayAffinity(hwnd, 0x00000001)  # WDA_MONITOR fallback
        return bool(result)
    except Exception as e:
        print(f"Error enabling screen protection: {e}")
        return False


def disable_screen_protection(hwnd: int) -> bool:
    try:
        if not hwnd:
            return False
        return bool(SetWindowDisplayAffinity(hwnd, WDA_NONE))
    except Exception as e:
        print(f"Error disabling screen protection: {e}")
        return False

