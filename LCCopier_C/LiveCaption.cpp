// LiveCaption.cpp : Defines the entry point for the application.
// Reads text from Windows built-in Live Caption and shows it in a styled text box.
//

#include "framework.h"
#include "LiveCaption.h"
#include <string>
#include <vector>
#include <algorithm>

#define MAX_LOADSTRING 100
#define IDT_POLL_CAPTION  1
#define IDT_PRINT_STATUS  2
#define POLL_INTERVAL_MS  400
#define PRINT_INTERVAL_MS 3000
#define IDHOTKEY_PASTE_SELECTION  1
#define WM_APP_DO_PASTE  (WM_APP + 2)
#define WM_APP_FIND_AND_COPY  (WM_APP + 3)

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HBRUSH g_hEditBrush = nullptr;                   // dark background for edit (Live Caption style)
HFONT  g_hCaptionFont = nullptr;                 // Segoe UI
static std::wstring g_lastCaptionText;            // only update edit when this changes (preserve cursor/scroll)
static std::wstring g_captionHistory;             // ALL captions accumulated since start (temp storage)
static std::wstring g_previousCaption;            // Previous caption text for comparison
static int          g_anchorCharIndex = 0;        // user-chosen start position in transcription
static std::wstring g_anchorPattern;              // unique pattern to find anchor point
static bool         g_anchorSetByUser = false;    // true if anchor was manually selected by user
static std::wstring g_selectionToEnd;            // text from anchor to end (updated on selection change)
static volatile long g_pasteInProgress = 0;      // re-entrancy guard for paste
static HWND g_hMainWnd = nullptr;
static HHOOK g_hKbHook = nullptr;

// Forward declarations
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
std::wstring        GetLiveCaptionText();
static BOOL CALLBACK FindLiveCaptionWindow(HWND hwnd, LPARAM lParam);
static bool CollectTextFromElement(IUIAutomation* pAutomation, IUIAutomationElement* pElement, std::wstring& out, bool skipRootName);
static bool IsUiChrome(const wchar_t* name);
static void UpdateSelectionFromAnchorToEnd(HWND hEdit, bool updatePattern = false);
static void ApplyYellowHighlight(HWND hEdit);
static void DoPasteWork();
static LRESULT CALLBACK LowLevelKbHook(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static std::wstring ExtractAnchorPattern(const std::wstring& text, int anchorPos);
static int FindAnchorByPattern(const std::wstring& text, const std::wstring& pattern);
static bool PasteViaClipboard(const std::wstring& text);
static void DoFindAndCopyWork();
static void PrintStatus();
static void UpdateCaptionHistory(const std::wstring& currentText);

// --- UI Automation: find Live Caption window and read its text ---
static BOOL CALLBACK FindLiveCaptionWindow(HWND hwnd, LPARAM lParam) {
    WCHAR title[256] = {};
    if (!GetWindowTextW(hwnd, title, (int)std::size(title))) return TRUE;
    std::wstring t(title);
    std::transform(t.begin(), t.end(), t.begin(), ::towlower);
    if (t.find(L"live caption") != std::wstring::npos) {
        *reinterpret_cast<HWND*>(lParam) = hwnd;
        return FALSE; // stop enumeration
    }
    return TRUE;
}

// Skip window title and other Live Caption UI labels - we only want the transcription.
static bool IsUiChrome(const wchar_t* name) {
    if (!name || !*name) return true;
    std::wstring s(name);
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    if (s.find(L"live caption") != std::wstring::npos) return true;
    if (s == L"settings" || s == L"position" || s == L"preferences") return true;
    if (s.find(L"caption style") != std::wstring::npos) return true;
    if (s.find(L"edit") == 0 && s.length() <= 5) return true; // "Edit" button
    return false;
}

// Returns true when we've found the transcription and set 'out' (caller should stop).
static bool CollectTextFromElement(IUIAutomation* pAutomation, IUIAutomationElement* pElement, std::wstring& out, bool skipRootName) {
    IUIAutomationTextPattern* pTextPattern = nullptr;
    HRESULT hr = pElement->GetCurrentPatternAs(UIA_TextPatternId, __uuidof(IUIAutomationTextPattern), reinterpret_cast<void**>(&pTextPattern));
    if (SUCCEEDED(hr) && pTextPattern) {
        IUIAutomationTextRange* pRange = nullptr;
        if (SUCCEEDED(pTextPattern->get_DocumentRange(&pRange)) && pRange) {
            BSTR bstr = nullptr;
            if (SUCCEEDED(pRange->GetText(-1, &bstr)) && bstr) {
                std::wstring candidate(bstr);
                SysFreeString(bstr);
                // Use this text only if it looks like transcription (not UI chrome)
                if (!candidate.empty() && !IsUiChrome(candidate.c_str())) {
                    out = candidate;
                    pRange->Release();
                    pTextPattern->Release();
                    return true; // found transcription, stop
                }
            }
            pRange->Release();
        }
        pTextPattern->Release();
    }
    BSTR name = nullptr;
    if (!skipRootName && SUCCEEDED(pElement->get_CurrentName(&name)) && name && *name) {
        if (!IsUiChrome(name)) {
            out += name;
            out += L"\r\n";
        }
        SysFreeString(name);
    }
    IUIAutomationTreeWalker* pWalker = nullptr;
    if (FAILED(pAutomation->get_ControlViewWalker(&pWalker)) || !pWalker) return false;
    IUIAutomationElement* pChild = nullptr;
    if (SUCCEEDED(pWalker->GetFirstChildElement(pElement, &pChild)) && pChild) {
        IUIAutomationElement* pNext = pChild;
        do {
            if (CollectTextFromElement(pAutomation, pNext, out, false)) {
                pNext->Release();
                if (pWalker) pWalker->Release();
                return true; // child found transcription
            }
            IUIAutomationElement* pSibling = nullptr;
            pWalker->GetNextSiblingElement(pNext, &pSibling);
            pNext->Release();
            pNext = pSibling;
        } while (pNext);
    }
    if (pWalker) pWalker->Release();
    return false;
}

// Scroll the edit so the bottom of the text is visible (keeps view at end as new text arrives).
static void ScrollEditToBottom(HWND hEdit) {
    if (!hEdit) return;
    int len = GetWindowTextLengthW(hEdit);
    if (len <= 0) return;
    SendMessageW(hEdit, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    SendMessageW(hEdit, EM_SCROLLCARET, 0, 0);
    SendMessageW(hEdit, WM_VSCROLL, SB_BOTTOM, 0);
}

// Apply yellow background to (g_anchorCharIndex, end). Rest is black on white. No blue selection highlight.
static void ApplyYellowHighlight(HWND hEdit) {
    if (!hEdit) return;
    int len = GetWindowTextLengthW(hEdit);
    if (len <= 0) return;
    g_anchorCharIndex = (std::min)(g_anchorCharIndex, len);
    CHARRANGE cr = {};
    CHARFORMAT2W cf = {};
    cf.cbSize = sizeof(cf);
    cr.cpMin = 0;
    cr.cpMax = -1;
    SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
    cf.dwMask = CFM_BACKCOLOR | CFM_COLOR;
    cf.crTextColor = RGB(0, 0, 0);
    cf.crBackColor = RGB(255, 255, 255);
    SendMessageW(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    cr.cpMin = g_anchorCharIndex;
    cr.cpMax = len;
    SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
    cf.dwMask = CFM_BACKCOLOR | CFM_COLOR;
    cf.crTextColor = RGB(0, 0, 0);
    cf.crBackColor = RGB(255, 220, 100);
    SendMessageW(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    cr.cpMin = len;
    cr.cpMax = len;
    SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
}

// Triggered on Ctrl+Shift+V key down only (via hook). Types g_selectionToEnd into foreground window; no clipboard.
static void DoPasteWork() {
    if (InterlockedCompareExchange(&g_pasteInProgress, 1, 0) != 0) return;
    PasteViaClipboard(g_selectionToEnd);
    InterlockedExchange(&g_pasteInProgress, 0);
}

// Extract a unique pattern from text around the anchor position (read backwards to find unique pattern)
static std::wstring ExtractAnchorPattern(const std::wstring& text, int anchorPos) {
    if (anchorPos <= 0 || anchorPos > (int)text.length()) return L"";

    // Start with minimum pattern length and increase until unique or max length
    const int MIN_PATTERN_LEN = 5;
    const int MAX_PATTERN_LEN = 50;

    for (int len = MIN_PATTERN_LEN; len <= MAX_PATTERN_LEN && len <= anchorPos; len++) {
        int start = anchorPos - len;
        std::wstring pattern = text.substr(start, len);

        // Check if this pattern is unique (appears only once before anchorPos)
        size_t firstOccurrence = text.find(pattern);
        size_t lastOccurrence = text.rfind(pattern, anchorPos - 1);

        if (firstOccurrence == lastOccurrence && firstOccurrence == (size_t)start) {
            return pattern; // Found unique pattern
        }
    }

    // If no unique pattern found, return the maximum length pattern
    int len = (std::min)(MAX_PATTERN_LEN, anchorPos);
    return text.substr(anchorPos - len, len);
}

// Find anchor position in text using the saved pattern
static int FindAnchorByPattern(const std::wstring& text, const std::wstring& pattern) {
    if (pattern.empty()) return 0;

    size_t pos = text.find(pattern);
    if (pos == std::wstring::npos) return 0;

    // Return position after the pattern
    return (int)(pos + pattern.length());
}

// NEW: Fast clipboard paste function
static bool PasteViaClipboard(const std::wstring& text) {
    if (text.empty()) return false;

    // Open clipboard
    if (!OpenClipboard(g_hMainWnd)) return false;

    // Save current clipboard content (optional - for restoration)
    HANDLE hOldData = GetClipboardData(CF_UNICODETEXT);
    std::wstring oldClipboard;
    if (hOldData) {
        wchar_t* pOldText = (wchar_t*)GlobalLock(hOldData);
        if (pOldText) {
            oldClipboard = pOldText;
            GlobalUnlock(hOldData);
        }
    }

    EmptyClipboard();

    // Allocate global memory for new text
    size_t size = (text.length() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!hMem) {
        CloseClipboard();
        return false;
    }

    // Copy text to global memory
    wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
    if (pMem) {
        wcscpy_s(pMem, text.length() + 1, text.c_str());
        GlobalUnlock(hMem);
    }

    // Set clipboard data
    SetClipboardData(CF_UNICODETEXT, hMem);
    CloseClipboard();

    // Simulate Ctrl+V to paste
    Sleep(10); // Small delay to ensure clipboard is ready
    INPUT inputs[4] = {};
    
    // Ctrl down
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CONTROL;
    
    // V down
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'V';
    
    // V up
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'V';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    
    // Ctrl up
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_CONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    
    SendInput(4, inputs, sizeof(INPUT));

    // Optional: Restore old clipboard after a delay
    // (You can do this in a background thread or skip it)
    Sleep(50); // Wait for paste to complete
    if (!oldClipboard.empty()) {
        if (OpenClipboard(g_hMainWnd)) {
            EmptyClipboard();
            size_t oldSize = (oldClipboard.length() + 1) * sizeof(wchar_t);
            HGLOBAL hOldMem = GlobalAlloc(GMEM_MOVEABLE, oldSize);
            if (hOldMem) {
                wchar_t* pOldMem = (wchar_t*)GlobalLock(hOldMem);
                if (pOldMem) {
                    wcscpy_s(pOldMem, oldClipboard.length() + 1, oldClipboard.c_str());
                    GlobalUnlock(hOldMem);
                    SetClipboardData(CF_UNICODETEXT, hOldMem);
                }
            }
            CloseClipboard();
        }
    }

    return true;
}

// Triggered on Ctrl+Shift+A: find anchor using pattern and copy from anchor to end
static void DoFindAndCopyWork() {
    if (InterlockedCompareExchange(&g_pasteInProgress, 1, 0) != 0) return;

    if (!g_anchorPattern.empty() && !g_captionHistory.empty()) {
        int foundPos = FindAnchorByPattern(g_captionHistory, g_anchorPattern);

        WCHAR debugMsg[512];
        swprintf_s(debugMsg, L"[Ctrl+Shift+A] Searching for pattern: \"%s\" | Found at position: %d | History length: %zu\n",
            g_anchorPattern.c_str(), foundPos, g_captionHistory.length());
        OutputDebugStringW(debugMsg);

        if (foundPos > 0 && foundPos <= (int)g_captionHistory.length()) {
            std::wstring textToCopy = g_captionHistory.substr(foundPos);

            swprintf_s(debugMsg, L"[Ctrl+Shift+A] Copying %zu chars from anchor to end\n", textToCopy.length());
            OutputDebugStringW(debugMsg);

            PasteViaClipboard(textToCopy);
        } else {
            OutputDebugStringW(L"[Ctrl+Shift+A] Pattern not found or invalid position!\n");
        }
    } else {
        OutputDebugStringW(L"[Ctrl+Shift+A] No anchor pattern set or history is empty!\n");
    }

    InterlockedExchange(&g_pasteInProgress, 0);
}

// Print the size of whole sentence and anchor pattern every 3 seconds
static void PrintStatus() {
    WCHAR msg[512];
    swprintf_s(msg, L"[Status] Whole sentence size: %zu chars | Anchor pattern: \"%s\"\n",
        g_captionHistory.length(),
        g_anchorPattern.empty() ? L"(none)" : g_anchorPattern.c_str());
    OutputDebugStringW(msg);
}

// Detect new words by finding a pattern from the end of previous text in current text
// Read backwards from previous text to find FIRST pattern that exists in current text
static void UpdateCaptionHistory(const std::wstring& currentText) {
    // First time - save everything
    if (g_previousCaption.empty()) {
        g_captionHistory = currentText;
        g_previousCaption = currentText;
        return;
    }

    size_t prevLen = g_previousCaption.length();
    size_t currLen = currentText.length();

    // If text got SHORTER, old text scrolled away - don't add anything
    if (currLen < prevLen) {
        g_previousCaption = currentText;
        return;
    }

    // If text didn't grow much, probably just punctuation edits - ignore
    if (currLen <= prevLen + 1) {
        g_previousCaption = currentText;
        return;
    }

    // Read backwards from end of previous text to find FIRST 20-char pattern that exists in current
    // Algorithm: Take last 20 chars, if not found, shift back 1 char and take next 20 chars, repeat
    const size_t patternLen = 20;

    bool foundPattern = false;
    std::wstring newPart;

    // Make sure we have enough text to extract a pattern
    if (prevLen < patternLen) {
        // Previous text too short, just append current as new sentence
        g_captionHistory += L" " + currentText;
        g_previousCaption = currentText;
        return;
    }

    // Start from the end and shift back one position at a time
    // endPos is the position AFTER the last char of the pattern (exclusive end)
    // First iteration: endPos = prevLen, pattern = [prevLen-20, prevLen-1]
    // Second iteration: endPos = prevLen-1, pattern = [prevLen-21, prevLen-2]
    // etc.
    size_t maxShift = (std::min)(prevLen - patternLen, (size_t)200); // Don't search more than 200 chars back

    // Convert current text to lowercase for case-insensitive comparison
    std::wstring currentLower = currentText;
    std::transform(currentLower.begin(), currentLower.end(), currentLower.begin(), ::towlower);
    std::wstring pattern;

    for (size_t shift = 0; shift <= maxShift; shift++) {
        size_t endPos = prevLen - shift;
        size_t startPos = endPos - patternLen;

        // Extract 20 chars from [startPos, endPos)
        pattern = g_previousCaption.substr(startPos, patternLen);

        // Convert pattern to lowercase for case-insensitive search
        std::wstring patternLower = pattern;
        std::transform(patternLower.begin(), patternLower.end(), patternLower.begin(), ::towlower);

        // Search for this pattern in current text (case-insensitive)
        size_t pos = currentLower.find(patternLower);

        if (pos != std::wstring::npos) {
            // Found the pattern! Append everything after it in current text
            newPart = currentText.substr(pos);
            foundPattern = true;
            break; // Stop at first match
        }
    }

    if (foundPattern) {
        // Append the new part after the pattern
        size_t hpos = g_captionHistory.find(pattern);

        if (hpos != std::wstring::npos) {
            // Keep everything before the pattern, remove pattern and everything after
            std::wstring historyBeforePattern = g_captionHistory.substr(0, hpos);
            g_captionHistory = historyBeforePattern + newPart;
        } else {
            g_captionHistory += newPart;
        }
    } else {
        // Pattern not found - treat as new sentence
        g_captionHistory += L" " + currentText;
    }

    g_previousCaption = currentText;
}

// Low-level keyboard hook: trigger paste on key *down* only (same as normal Ctrl+V), and consume the key.
static LRESULT CALLBACK LowLevelKbHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN && g_hMainWnd) {
        auto* p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

        // Ctrl+Shift+V: paste selection
        if (p->vkCode == 'V' && ctrlDown && shiftDown) {
            PostMessageW(g_hMainWnd, WM_APP_DO_PASTE, 0, 0);
            return 1;
        }

        // Ctrl+Shift+A: find anchor and copy
        if (p->vkCode == 'A' && ctrlDown && shiftDown) {
            PostMessageW(g_hMainWnd, WM_APP_FIND_AND_COPY, 0, 0);
            return 1;
        }
    }
    return CallNextHookEx(g_hKbHook, nCode, wParam, lParam);
}

// Set g_selectionToEnd and apply yellow highlight. No clipboard.
// updatePattern: only extract pattern if true (when user manually selects)
static void UpdateSelectionFromAnchorToEnd(HWND hEdit, bool updatePattern) {
    if (!hEdit) return;
    int len = GetWindowTextLengthW(hEdit);
    if (len <= 0) { g_selectionToEnd.clear(); return; }
    g_anchorCharIndex = (std::min)(g_anchorCharIndex, len);
    std::vector<wchar_t> buf(len + 1);
    if (GetWindowTextW(hEdit, buf.data(), len + 1) > 0) {
        g_selectionToEnd.assign(buf.data() + g_anchorCharIndex, buf.data() + len);

        // Extract anchor pattern ONLY when user manually selects with mouse
        if (updatePattern && g_anchorCharIndex > 0 && !g_captionHistory.empty()) {
            g_anchorPattern = ExtractAnchorPattern(g_captionHistory, g_anchorCharIndex);
        }
    }
    ApplyYellowHighlight(hEdit);
}

static WNDPROC g_origEditProc = nullptr;

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_LBUTTONUP) {
        LRESULT r = CallWindowProcW(g_origEditProc, hWnd, uMsg, wParam, lParam);
        CHARRANGE cr = {};
        SendMessageW(hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
        g_anchorCharIndex = (std::min)((int)cr.cpMin, (int)cr.cpMax);
        g_anchorSetByUser = true;  // User manually selected the anchor
        UpdateSelectionFromAnchorToEnd(hWnd, true);  // true = update pattern (mouse selection)

        // Print when mouse selects anchor
        OutputDebugStringW(L"Mouse selected\n");
        WCHAR msg[512];
        swprintf_s(msg, L"Anchor pattern: \"%s\"\n",
            g_anchorPattern.empty() ? L"(none)" : g_anchorPattern.c_str());
        OutputDebugStringW(msg);

        return r;
    }
    if (uMsg == WM_KEYDOWN && wParam == 'V' && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000)) {
        PostMessageW(GetParent(hWnd), WM_APP_DO_PASTE, 0, 0);
        return 0;
    }
    if (uMsg == WM_KEYDOWN && wParam == 'A' && (GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000)) {
        PostMessageW(GetParent(hWnd), WM_APP_FIND_AND_COPY, 0, 0);
        return 0;
    }
    return CallWindowProcW(g_origEditProc, hWnd, uMsg, wParam, lParam);
}

std::wstring GetLiveCaptionText() {
    HWND hwndCaption = nullptr;
    EnumWindows(FindLiveCaptionWindow, reinterpret_cast<LPARAM>(&hwndCaption));
    if (!hwndCaption) return L"";

    IUIAutomation* pAutomation = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), reinterpret_cast<void**>(&pAutomation));
    if (FAILED(hr) || !pAutomation) return L"";

    IUIAutomationElement* pRoot = nullptr;
    hr = pAutomation->ElementFromHandle(hwndCaption, &pRoot);
    if (FAILED(hr) || !pRoot) { pAutomation->Release(); return L""; }

    std::wstring text;
    CollectTextFromElement(pAutomation, pRoot, text, true); // true = skip root name ("Live captions")
    pRoot->Release();
    pAutomation->Release();

    while (!text.empty() && (text.back() == L'\r' || text.back() == L'\n')) text.pop_back();
    return text;
}

// --- App entry and window ---
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LIVECAPTION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LIVECAPTION));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    CoUninitialize();
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LIVECAPTION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LIVECAPTION);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 640, 320, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            // Black text on white background, Segoe UI
            HDC hdc = GetDC(hWnd);
            int logPixels = hdc ? GetDeviceCaps(hdc, LOGPIXELSY) : 96;
            if (hdc) ReleaseDC(hWnd, hdc);
            g_hCaptionFont = CreateFontW(
                -MulDiv(12, logPixels, 72), // 12pt
                0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
            g_hEditBrush = CreateSolidBrush(RGB(255, 255, 255));

            LoadLibraryW(L"Msftedit.dll");
            HWND hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"RICHEDIT50W", nullptr,
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                0, 0, 0, 0, hWnd, (HMENU)(INT_PTR)IDC_CAPTION_EDIT, hInst, nullptr);
            if (hEdit) {
                SendMessageW(hEdit, EM_SETBKGNDCOLOR, 0, (LPARAM)RGB(255, 255, 255));
                SendMessageW(hEdit, WM_SETFONT, (WPARAM)g_hCaptionFont, TRUE);
                g_origEditProc = (WNDPROC)SetWindowLongPtrW(hEdit, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
                SetTimer(hWnd, IDT_POLL_CAPTION, POLL_INTERVAL_MS, nullptr);
                SetTimer(hWnd, IDT_PRINT_STATUS, PRINT_INTERVAL_MS, nullptr);
            }
            g_hMainWnd = hWnd;
            g_hKbHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKbHook, nullptr, 0);
        }
        break;
    case WM_APP_DO_PASTE:
        DoPasteWork();
        return 0;
    case WM_APP_FIND_AND_COPY:
        DoFindAndCopyWork();
        return 0;
    case WM_SIZE:
        {
            HWND hEdit = GetDlgItem(hWnd, IDC_CAPTION_EDIT);
            if (hEdit) SetWindowPos(hEdit, nullptr, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER);
        }
        break;
    case WM_CTLCOLOREDIT:
        {
            SetTextColor((HDC)wParam, RGB(0, 0, 0));
            SetBkColor((HDC)wParam, RGB(255, 255, 255));
            return (LRESULT)g_hEditBrush;
        }
    case WM_TIMER:
        if (wParam == IDT_POLL_CAPTION) {
            std::wstring text = GetLiveCaptionText();
            if (text != g_lastCaptionText) {
                // Update caption history by detecting new words
                if (!text.empty()) {
                    UpdateCaptionHistory(text);
                }

                g_lastCaptionText = std::move(text);

                HWND hEdit = GetDlgItem(hWnd, IDC_CAPTION_EDIT);
                if (hEdit) {
                    SetWindowTextW(hEdit, g_lastCaptionText.c_str());

                    // Mark anchor as auto-set when text updates (unless user has manually set it)
                    if (!g_anchorSetByUser) {
                        g_anchorCharIndex = 0;  // Auto anchor at start
                    }

                    UpdateSelectionFromAnchorToEnd(hEdit, false);
                    ScrollEditToBottom(hEdit);
                }
            }
        }
        else if (wParam == IDT_PRINT_STATUS) {
            PrintStatus();
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        if (g_hKbHook) { UnhookWindowsHookEx(g_hKbHook); g_hKbHook = nullptr; }
        KillTimer(hWnd, IDT_POLL_CAPTION);
        KillTimer(hWnd, IDT_PRINT_STATUS);
        if (g_hEditBrush) { DeleteObject(g_hEditBrush); g_hEditBrush = nullptr; }
        if (g_hCaptionFont) { DeleteObject(g_hCaptionFont); g_hCaptionFont = nullptr; }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
