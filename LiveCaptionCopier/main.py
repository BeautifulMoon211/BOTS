"""
Stealth Live Caption Viewer
Displays Windows Live Captions in a stealth window
"""

import sys
from html import escape
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QFrame, QSlider, QLabel, QTextEdit
)
from PyQt6.QtCore import Qt, QTimer
from PyQt6.QtGui import QFont, QTextCursor
import keyboard

from screen_protection import enable_screen_protection, disable_screen_protection
from taskbar_helper import hide_from_taskbar, show_in_taskbar
from caption_reader import LiveCaptionReader

# Style constants
BTN_OFF = "QPushButton{background:#2d2d4a;border:1px solid #3d3d5c;border-radius:2px;color:#a0a0c0;padding:0 4px;font-size:10px;max-height:20px}QPushButton:hover{background:#3d3d5c}"
BTN_ON = "QPushButton{background:#2d6d4a;border:1px solid #3d8d5a;border-radius:2px;color:#e0ffe0;padding:0 4px;font-size:10px;max-height:20px}QPushButton:hover{background:#3d7d5a}"


class CaptionDisplay(QTextEdit):
    """Caption display with click-to-mark feature."""
    
    MAX_CHARS = 10000
    
    def __init__(self):
        super().__init__()
        self.setReadOnly(True)
        self.marked_word = None
        self.full_text = ""
        self.setStyleSheet("QTextEdit{background:#fff;color:#000;border:none;font-size:13px;font-family:'Segoe UI';padding:10px}")
        self.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAsNeeded)
        self.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        # Use global override cursor when mouse enters
        self.setMouseTracking(True)
        self.viewport().setMouseTracking(True)
    
    def enterEvent(self, event):
        # Only set cursor if not already overridden to prevent stack corruption
        if not QApplication.overrideCursor():
            QApplication.setOverrideCursor(Qt.CursorShape.ArrowCursor)
        super().enterEvent(event)

    def leaveEvent(self, event):
        # Only restore if cursor is currently overridden
        if QApplication.overrideCursor():
            QApplication.restoreOverrideCursor()
        super().leaveEvent(event)
    
    def mouseReleaseEvent(self, event):
        if event.button() == Qt.MouseButton.LeftButton:
            pos = min(self.cursorForPosition(event.pos()).position(), len(self.toPlainText()))
            word = self._get_word_context(self.toPlainText(), pos)
            if word:
                self.marked_word = None if self.marked_word == word else word
                self._refresh()
        super().mouseReleaseEvent(event)
    
    def _get_word_context(self, text, pos):
        """Get word at position with context for unique ID."""
        if not text or pos >= len(text):
            return None
        # Find word start
        start = pos
        while start > 0 and not text[start - 1].isspace():
            start -= 1
        # Skip whitespace
        while start < len(text) and text[start].isspace():
            start += 1
        return text[start:start + 40] if start < len(text) else None
    
    def set_text(self, text):
        """Set text, trimming if needed."""
        if len(text) > self.MAX_CHARS:
            trim = len(text) - self.MAX_CHARS
            while trim < len(text) and not text[trim].isspace():
                trim += 1
            # Ensure we don't go out of bounds
            if trim + 1 < len(text):
                text = text[trim + 1:]
            else:
                text = text[trim:] if trim < len(text) else text[-self.MAX_CHARS:]
        self.full_text = text
        self._refresh()
    
    def _refresh(self):
        """Update display preserving scroll."""
        if not self.full_text:
            return
        scroll = self.verticalScrollBar().value()
        self.blockSignals(True)

        # Safely handle marked_word slicing with None check
        mark_pos = -1
        if self.marked_word:
            search_str = self.marked_word[:20] if len(self.marked_word) >= 20 else self.marked_word
            mark_pos = self.full_text.find(search_str)

        if mark_pos < 0:
            self.marked_word = None

        if mark_pos >= 0:
            before = escape(self.full_text[:mark_pos]).replace('\n', '<br>')
            after = escape(self.full_text[mark_pos:]).replace('\n', '<br>')
            self.setHtml(f'<span style="color:#999">{before}</span><span style="color:#228B22">{after}</span>')
        else:
            self.setPlainText(self.full_text)

        self.blockSignals(False)
        self.verticalScrollBar().setValue(scroll)
    
    def get_marked_text(self):
        """Get text from mark to end."""
        if self.marked_word:
            search_str = self.marked_word[:20] if len(self.marked_word) >= 20 else self.marked_word
            pos = self.full_text.find(search_str)
            if pos >= 0:
                return self.full_text[pos:]
        return self.full_text


class StealthCaptionApp(QMainWindow):
    """Main application window."""
    
    def __init__(self):
        super().__init__()
        self.screen_protection_enabled = False
        self.taskbar_hidden = False
        self.always_on_top = False
        self.caption_reader = LiveCaptionReader()
        self.last_caption = ""
        self.hwnd = None
        
        self._init_ui()
        # Fixed global hotkey: Ctrl+Shift+C
        try:
            self.hotkey_handle = keyboard.add_hotkey('ctrl+shift+c', self._on_global_copy, suppress=True)
        except Exception as e:
            print(f"Warning: Failed to register global hotkey Ctrl+Shift+C: {e}")
            self.hotkey_handle = None
        
        self.timer = QTimer(self)
        self.timer.timeout.connect(self._poll_captions)
        self.timer.start(100)
        
        QTimer.singleShot(100, self._init_hwnd)
    
    def _init_ui(self):
        self.setWindowTitle("Caption")
        self.resize(500, 180)
        self.setStyleSheet("QMainWindow{background:#0f0f1a}")
        
        central = QWidget()
        self.setCentralWidget(central)
        layout = QVBoxLayout(central)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        
        # Control bar
        bar = QFrame()
        bar.setFixedHeight(26)
        bar.setStyleSheet("QFrame{background:#1a1a2e;border-bottom:1px solid #2d2d44}")
        bar_layout = QHBoxLayout(bar)
        bar_layout.setContentsMargins(2, 0, 2, 0)
        bar_layout.setSpacing(2)
        
        self.status_label = QLabel("⏳ Waiting...")
        self.status_label.setStyleSheet("color:#808090;font-size:10px;padding:0 8px")
        bar_layout.addWidget(self.status_label)
        bar_layout.addStretch()
        
        # Buttons
        self.btn_copy = self._make_btn("📋", self.copy_from_mark)
        bar_layout.addWidget(self.btn_copy)
        
        self.btn_protect = self._make_btn("🛡️ OFF", self.toggle_screen_protection)
        self.btn_taskbar = self._make_btn("📌 Show", self.toggle_taskbar_visibility)
        self.btn_top = self._make_btn("📍 Off", self.toggle_always_on_top)
        
        for btn in [self.btn_protect, self.btn_taskbar, self.btn_top]:
            bar_layout.addWidget(btn)
        
        # Opacity slider
        lbl = QLabel("🔆")
        lbl.setStyleSheet("color:#a0a0c0;font-size:10px")
        bar_layout.addWidget(lbl)
        
        slider = QSlider(Qt.Orientation.Horizontal)
        slider.setFixedWidth(60)
        slider.setRange(30, 100)
        slider.setValue(100)
        slider.setStyleSheet("QSlider::groove:horizontal{background:#2d2d4a;height:4px}QSlider::handle:horizontal{background:#6d6dac;width:10px;margin:-3px 0;border-radius:5px}")
        slider.valueChanged.connect(lambda v: self.setWindowOpacity(v / 100))
        bar_layout.addWidget(slider)
        
        layout.addWidget(bar)
        
        self.caption_display = CaptionDisplay()
        layout.addWidget(self.caption_display)
    
    def _make_btn(self, text, callback):
        btn = QPushButton(text)
        btn.setStyleSheet(BTN_OFF)
        btn.clicked.connect(callback)
        return btn
    
    def _init_hwnd(self):
        self.hwnd = int(self.winId())
        # Enable stealth by default
        self.toggle_screen_protection()
        self.toggle_taskbar_visibility()
        self.toggle_always_on_top()
    
    def _on_global_copy(self):
        # Use QTimer to safely call from keyboard thread
        QTimer.singleShot(0, self._copy_and_paste)
    
    def _copy_and_paste(self):
        """Copy marked text and paste to active window."""
        text = self.caption_display.get_marked_text()
        if text:
            QApplication.clipboard().setText(text)
            self._set_status("📋 Pasted!", "#a78bfa")
            # Small delay then paste
            QTimer.singleShot(50, lambda: keyboard.press_and_release('ctrl+v'))
    
    def _poll_captions(self):
        try:
            text = self.caption_reader.get_caption_text()
            if text and text != self.last_caption:
                self.last_caption = text
                self.caption_display.set_text(text)
                cursor = self.caption_display.textCursor()
                cursor.movePosition(QTextCursor.MoveOperation.End)
                self.caption_display.setTextCursor(cursor)
                self._set_status("🎤 Live", "#4ade80")
            elif not text:
                if self.caption_reader.is_available():
                    self._set_status("🎤 Listening...", "#60a5fa")
                else:
                    self._set_status("⚠️ Win+Ctrl+L", "#f59e0b")
        except Exception as e:
            print(f"Error polling captions: {e}")
            self._set_status("❌ Error", "#ef4444")
    
    def _set_status(self, text, color):
        self.status_label.setText(text)
        self.status_label.setStyleSheet(f"color:{color};font-size:10px;padding:0 8px")
    
    def copy_from_mark(self):
        text = self.caption_display.get_marked_text()
        if text:
            QApplication.clipboard().setText(text)
            self._set_status("📋 Copied!", "#a78bfa")
    
    def toggle_screen_protection(self):
        if not self.hwnd:
            self.hwnd = int(self.winId())
        self.screen_protection_enabled = not self.screen_protection_enabled
        if self.screen_protection_enabled:
            enable_screen_protection(self.hwnd)
            self.btn_protect.setText("🛡️ ON")
            self.btn_protect.setStyleSheet(BTN_ON)
        else:
            disable_screen_protection(self.hwnd)
            self.btn_protect.setText("🛡️ OFF")
            self.btn_protect.setStyleSheet(BTN_OFF)
    
    def toggle_taskbar_visibility(self):
        if not self.hwnd:
            self.hwnd = int(self.winId())
        self.taskbar_hidden = not self.taskbar_hidden
        if self.taskbar_hidden:
            hide_from_taskbar(self.hwnd)
            self.btn_taskbar.setText("📌 Hide")
            self.btn_taskbar.setStyleSheet(BTN_ON)
        else:
            show_in_taskbar(self.hwnd)
            self.btn_taskbar.setText("📌 Show")
            self.btn_taskbar.setStyleSheet(BTN_OFF)
    
    def toggle_always_on_top(self):
        self.always_on_top = not self.always_on_top
        if self.always_on_top:
            self.setWindowFlags(self.windowFlags() | Qt.WindowType.WindowStaysOnTopHint)
            self.btn_top.setText("📍 On")
            self.btn_top.setStyleSheet(BTN_ON)
        else:
            self.setWindowFlags(self.windowFlags() & ~Qt.WindowType.WindowStaysOnTopHint)
            self.btn_top.setText("📍 Off")
            self.btn_top.setStyleSheet(BTN_OFF)
        self.show()
        # Wait longer for window recreation and verify HWND
        QTimer.singleShot(200, self._reapply_settings)
    
    def _reapply_settings(self):
        try:
            new_hwnd = int(self.winId())
            # Verify HWND is valid (non-zero)
            if new_hwnd:
                self.hwnd = new_hwnd
                if self.screen_protection_enabled:
                    enable_screen_protection(self.hwnd)
                if self.taskbar_hidden:
                    hide_from_taskbar(self.hwnd)
            else:
                print("Warning: Invalid HWND after window recreation")
        except Exception as e:
            print(f"Error reapplying settings: {e}")
    
    def closeEvent(self, event):
        try:
            if self.hotkey_handle is not None:
                keyboard.remove_hotkey(self.hotkey_handle)
        except Exception as e:
            print(f"Warning: Failed to remove hotkey: {e}")
        event.accept()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setFont(QFont("Segoe UI", 10))
    window = StealthCaptionApp()
    window.show()
    sys.exit(app.exec())
