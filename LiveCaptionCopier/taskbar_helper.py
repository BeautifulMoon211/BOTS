"""
Windows Taskbar Helper Module
"""

import ctypes
from ctypes import wintypes

GWL_EXSTYLE = -20
WS_EX_TOOLWINDOW = 0x00000080
WS_EX_APPWINDOW = 0x00040000

user32 = ctypes.windll.user32

GetWindowLongPtrW = user32.GetWindowLongPtrW
GetWindowLongPtrW.argtypes = [wintypes.HWND, ctypes.c_int]
GetWindowLongPtrW.restype = ctypes.c_long

SetWindowLongPtrW = user32.SetWindowLongPtrW
SetWindowLongPtrW.argtypes = [wintypes.HWND, ctypes.c_int, ctypes.c_long]
SetWindowLongPtrW.restype = ctypes.c_long

ShowWindow = user32.ShowWindow
ShowWindow.argtypes = [wintypes.HWND, ctypes.c_int]
ShowWindow.restype = wintypes.BOOL


def hide_from_taskbar(hwnd: int) -> bool:
    try:
        style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE)
        new_style = (style & ~WS_EX_APPWINDOW) | WS_EX_TOOLWINDOW
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, new_style)
        ShowWindow(hwnd, 0)
        ShowWindow(hwnd, 5)
        return True
    except:
        return False


def show_in_taskbar(hwnd: int) -> bool:
    try:
        style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE)
        new_style = (style | WS_EX_APPWINDOW) & ~WS_EX_TOOLWINDOW
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, new_style)
        ShowWindow(hwnd, 0)
        ShowWindow(hwnd, 5)
        return True
    except:
        return False

