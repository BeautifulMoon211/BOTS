#include "LiveCaption.h"
#include "SettingsDialog.h"

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HBRUSH g_hEditBrush = nullptr;
HFONT g_hCaptionFont = nullptr;
static std::wstring g_lastCaptionText;
static std::wstring g_captionHistory;
static std::wstring g_previousCaption;
static int g_anchorCharIndex = 0;
static int g_anchorHistoryIndex = 0;
static bool g_anchorSetByUser = false;
static volatile long g_pasteInProgress = 0;
static HWND g_hMainWnd = nullptr;
static HHOOK g_hKbHook = nullptr;
static bool g_userScrolledUp = false;
static HotkeyConfig g_autoCopyHotkey   = { true, true, false, false, 'A' };
static HotkeyConfig g_autoDeleteHotkey = { true, true, false, false, 'D' };
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
std::wstring GetLiveCaptionText();
static BOOL CALLBACK FindLiveCaptionWindow(HWND hwnd, LPARAM lParam);
static bool CollectTextFromElement(IUIAutomation* pAutomation, IUIAutomationElement* pElement, std::wstring& out, bool skipRootName);
static bool IsUiChrome(const wchar_t* name);
static void ApplyYellowHighlight(HWND hEdit);
static LRESULT CALLBACK LowLevelKbHook(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static bool PasteViaClipboard(const std::wstring& text);
static void DoFindAndCopyWork();
static void UpdateCaptionHistory(const std::wstring& currentText);

static BOOL CALLBACK FindLiveCaptionWindow(HWND hwnd, LPARAM lParam) {
	WCHAR title[256] = {};
	if (!GetWindowTextW(hwnd, title, (int)std::size(title))) return TRUE;
	std::wstring t(title);
	std::transform(t.begin(), t.end(), t.begin(), ::towlower);
	if (t.find(L"live caption") != std::wstring::npos) {
		*reinterpret_cast<HWND*>(lParam) = hwnd;
		return FALSE;
	}
	return TRUE;
}

static bool IsUiChrome(const wchar_t* name) {
	if (!name || !*name) return true;
	std::wstring s(name);
	std::transform(s.begin(), s.end(), s.begin(), ::towlower);
	if (s.find(L"live caption") != std::wstring::npos) return true;
	if (s == L"settings" || s == L"position" || s == L"preferences") return true;
	if (s.find(L"caption style") != std::wstring::npos) return true;
	if (s.find(L"edit") == 0 && s.length() <= 5) return true;
	return false;
}

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
				if (!candidate.empty() && !IsUiChrome(candidate.c_str())) {
					out = candidate;
					pRange->Release();
					pTextPattern->Release();
					return true;
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
				return true;
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

static bool IsScrolledToBottom(HWND hEdit) {
	if (!hEdit) return true;
	SCROLLINFO si = {};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	if (!GetScrollInfo(hEdit, SB_VERT, &si)) return true;
	int maxScroll = si.nMax - (int)si.nPage + 1;
	return (si.nPos >= maxScroll - 5);
}

static void ScrollEditToBottom(HWND hEdit) {
	if (!hEdit) return;
	if (g_userScrolledUp) return;
	int len = GetWindowTextLengthW(hEdit);
	if (len <= 0) return;
	SendMessageW(hEdit, EM_SETSEL, (WPARAM)len, (LPARAM)len);
	SendMessageW(hEdit, EM_SCROLLCARET, 0, 0);
	SendMessageW(hEdit, WM_VSCROLL, SB_BOTTOM, 0);
}

static void ApplyYellowHighlight(HWND hEdit) {
	if (!hEdit) return;
	int len = GetWindowTextLengthW(hEdit);
	if (len <= 0) return;
	g_anchorCharIndex = (std::min)(g_anchorCharIndex, len);
	AppSettings settings = SettingsDialog::LoadSettings();
	POINT ptScroll = {};
	SendMessageW(hEdit, EM_GETSCROLLPOS, 0, (LPARAM)&ptScroll);
	SendMessageW(hEdit, WM_SETREDRAW, FALSE, 0);
	CHARRANGE cr = {};
	CHARFORMAT2W cf = {};
	cf.cbSize = sizeof(cf);
	cr.cpMin = 0;
	cr.cpMax = g_anchorCharIndex;
	SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
	cf.dwMask = CFM_BACKCOLOR | CFM_COLOR;
	cf.crTextColor = settings.textColor;
	cf.crBackColor = settings.bgColor;
	SendMessageW(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	cr.cpMin = g_anchorCharIndex;
	cr.cpMax = len;
	SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
	cf.dwMask = CFM_BACKCOLOR | CFM_COLOR;
	cf.crTextColor = settings.textColor;
	cf.crBackColor = settings.selectedBgColor;
	SendMessageW(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	cr.cpMin = g_anchorCharIndex;
	cr.cpMax = g_anchorCharIndex;
	SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&cr);
	SendMessageW(hEdit, EM_SETSCROLLPOS, 0, (LPARAM)&ptScroll);
	SendMessageW(hEdit, WM_SETREDRAW, TRUE, 0);
	InvalidateRect(hEdit, nullptr, TRUE);
}

static bool PasteViaClipboard(const std::wstring& text) {
	if (text.empty()) return false;
	if (!OpenClipboard(g_hMainWnd)) return false;
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
	size_t size = (text.length() + 1) * sizeof(wchar_t);
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
	if (!hMem) {
		CloseClipboard();
		return false;
	}
	wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
	if (pMem) {
		wcscpy_s(pMem, text.length() + 1, text.c_str());
		GlobalUnlock(hMem);
	}
	SetClipboardData(CF_UNICODETEXT, hMem);
	CloseClipboard();
	Sleep(10);
	INPUT inputs[4] = {};
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_CONTROL;
	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = 'V';
	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = 'V';
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wVk = VK_CONTROL;
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(4, inputs, sizeof(INPUT));
	Sleep(50);
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
static void DoFindAndCopyWork() {
	if (InterlockedCompareExchange(&g_pasteInProgress, 1, 0) != 0) return;
	try {
		if (!g_captionHistory.empty()) {
			if (g_anchorHistoryIndex >= 0 && g_anchorHistoryIndex < (int)g_captionHistory.length()) {
				std::wstring textToCopy = g_captionHistory.substr(g_anchorHistoryIndex);
				PasteViaClipboard(textToCopy);
			}
		}
	}
	catch (...) {
	}
	InterlockedExchange(&g_pasteInProgress, 0);
}

static int FindWordStart(const std::wstring& text, int pos) {
	if (text.empty() || pos <= 0) return 0;
	if (pos >= (int)text.length()) pos = (int)text.length() - 1;
	while (pos > 0) {
		wchar_t ch = text[pos - 1];
		if (ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n' || ch == L'.' || ch == L',' || ch == L'!' || ch == L'?') {
			break;
		}
		pos--;
	}
	return pos;
}

static void AutoStartLiveCaption() {
	INPUT inputs[6] = {};
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_LWIN;
	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_CONTROL;
	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = 'L';
	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wVk = 'L';
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
	inputs[4].type = INPUT_KEYBOARD;
	inputs[4].ki.wVk = VK_CONTROL;
	inputs[4].ki.dwFlags = KEYEVENTF_KEYUP;
	inputs[5].type = INPUT_KEYBOARD;
	inputs[5].ki.wVk = VK_LWIN;
	inputs[5].ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(6, inputs, sizeof(INPUT));
}

static void DoClearHistory() {
	std::wstring currentLiveCaption = GetLiveCaptionText();
	g_captionHistory.clear();
	g_previousCaption = currentLiveCaption;
	g_lastCaptionText = currentLiveCaption;
	g_anchorCharIndex = 0;
	g_anchorHistoryIndex = 0;
	g_anchorSetByUser = false;
	HWND hEdit = GetDlgItem(g_hMainWnd, IDC_CAPTION_EDIT);
	if (hEdit) {
		SendMessageW(hEdit, WM_SETREDRAW, FALSE, 0);
		SetWindowTextW(hEdit, L"");
		SendMessageW(hEdit, WM_SETREDRAW, TRUE, 0);
		InvalidateRect(hEdit, nullptr, TRUE);
	}
}

static void UpdateCaptionHistory(const std::wstring& currentText) {
	if (g_previousCaption.empty()) {
		g_captionHistory = currentText;
		g_previousCaption = currentText;
		return;
	}
	size_t prevLen = g_previousCaption.length();
	size_t currLen = currentText.length();
	if (currLen < prevLen) {
		g_previousCaption = currentText;
		return;
	}
	if (currLen <= prevLen + 1) {
		g_previousCaption = currentText;
		return;
	}
	const size_t patternLen = 20;
	bool foundPattern = false;
	std::wstring newPart;
	if (prevLen < patternLen) {
		g_captionHistory = currentText;
		g_previousCaption = currentText;
		return;
	}
	size_t maxShift = (std::min)(prevLen - patternLen, (size_t)200);
	std::wstring currentLower = currentText;
	std::transform(currentLower.begin(), currentLower.end(), currentLower.begin(), ::towlower);
	std::wstring pattern;

	for (size_t shift = 0; shift <= maxShift; shift++) {
		size_t endPos = prevLen - shift;
		size_t startPos = endPos - patternLen;
		pattern = g_previousCaption.substr(startPos, patternLen);
		std::wstring patternLower = pattern;
		std::transform(patternLower.begin(), patternLower.end(), patternLower.begin(), ::towlower);
		size_t pos = currentLower.find(patternLower);
		if (pos != std::wstring::npos) {
			newPart = currentText.substr(pos);
			foundPattern = true;
			break;
		}
	}
	if (foundPattern) {
		size_t hpos = g_captionHistory.rfind(pattern);
		if (hpos != std::wstring::npos) {
			std::wstring historyBeforePattern = g_captionHistory.substr(0, hpos);
			g_captionHistory = historyBeforePattern + newPart;
		}
		else {
			g_captionHistory += newPart;
		}
	}
	else {
		g_captionHistory += L" " + currentText;
	}
	g_previousCaption = currentText;
}

static LRESULT CALLBACK LowLevelKbHook(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION && wParam == WM_KEYDOWN && g_hMainWnd) {
		auto* p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
		bool ctrlDown  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
		bool shiftDown = (GetAsyncKeyState(VK_SHIFT)   & 0x8000) != 0;
		bool altDown   = (GetAsyncKeyState(VK_MENU)    & 0x8000) != 0;
		bool winDown   = ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) != 0;
		auto matches = [&](const HotkeyConfig& hk) {
			return hk.vkCode == p->vkCode
				&& hk.ctrl  == ctrlDown  && hk.shift == shiftDown
				&& hk.alt   == altDown   && hk.win   == winDown;
		};
		if (matches(g_autoCopyHotkey)) {
			PostMessageW(g_hMainWnd, WM_APP_FIND_AND_COPY, 0, 0);
			return 1;
		}
		if (matches(g_autoDeleteHotkey)) {
			PostMessageW(g_hMainWnd, WM_APP_CLEAR_HISTORY, 0, 0);
			return 1;
		}
	}
	return CallNextHookEx(g_hKbHook, nCode, wParam, lParam);
}

static WNDPROC g_origEditProc = nullptr;

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_MOUSEWHEEL || uMsg == WM_VSCROLL ||
		(uMsg == WM_KEYDOWN && (wParam == VK_UP || wParam == VK_DOWN || wParam == VK_PRIOR || wParam == VK_NEXT))) {
		LRESULT r = CallWindowProcW(g_origEditProc, hWnd, uMsg, wParam, lParam);
		bool atBottom = IsScrolledToBottom(hWnd);
		if (atBottom && g_userScrolledUp) {
			g_userScrolledUp = false;
		}
		else if (!atBottom && !g_userScrolledUp) {
			g_userScrolledUp = true;
		}
		return r;
	}
	if (uMsg == WM_LBUTTONDOWN) {
		LRESULT r = CallWindowProcW(g_origEditProc, hWnd, uMsg, wParam, lParam);
		CHARRANGE cr = {};
		SendMessageW(hWnd, EM_EXGETSEL, 0, (LPARAM)&cr);
		int clickPos = (std::min)((int)cr.cpMin, (int)cr.cpMax);
		int wordStart = FindWordStart(g_captionHistory, clickPos);
		g_anchorCharIndex = wordStart;
		g_anchorSetByUser = true;
		g_anchorHistoryIndex = wordStart;
		ApplyYellowHighlight(hWnd);
		return r;
	}
	if (uMsg == WM_KEYDOWN) {
		bool ctrl  = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
		bool shift = (GetKeyState(VK_SHIFT)   & 0x8000) != 0;
		bool alt   = (GetKeyState(VK_MENU)    & 0x8000) != 0;
		bool win   = ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) != 0;
		auto matches = [&](const HotkeyConfig& hk) {
			return hk.vkCode == (UINT)wParam
				&& hk.ctrl  == ctrl  && hk.shift == shift
				&& hk.alt   == alt   && hk.win   == win;
		};
		if (matches(g_autoCopyHotkey)) {
			PostMessageW(GetParent(hWnd), WM_APP_FIND_AND_COPY, 0, 0);
			return 0;
		}
		if (matches(g_autoDeleteHotkey)) {
			PostMessageW(GetParent(hWnd), WM_APP_CLEAR_HISTORY, 0, 0);
			return 0;
		}
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
	CollectTextFromElement(pAutomation, pRoot, text, true);
	pRoot->Release();
	pAutomation->Release();
	while (!text.empty() && (text.back() == L'\r' || text.back() == L'\n')) text.pop_back();
	return text;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LIVECAPTION, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}
	AutoStartLiveCaption();
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LIVECAPTION));
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	CoUninitialize();
	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LIVECAPTION));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance;
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, 0, 640, 320, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) {
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		AppSettings settings = SettingsDialog::LoadSettings();
		HDC hdc = GetDC(hWnd);
		int logPixels = hdc ? GetDeviceCaps(hdc, LOGPIXELSY) : 96;
		if (hdc) ReleaseDC(hWnd, hdc);
		g_hCaptionFont = CreateFontW(
			-MulDiv(settings.textSize, logPixels, 72),
			0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
			DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
		g_hEditBrush = CreateSolidBrush(settings.bgColor);

		LoadLibraryW(L"Msftedit.dll");
		HWND hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"RICHEDIT50W", nullptr,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
			0, 0, 0, 0, hWnd, (HMENU)(INT_PTR)IDC_CAPTION_EDIT, hInst, nullptr);
		if (hEdit) {
			SendMessageW(hEdit, EM_SETBKGNDCOLOR, 0, (LPARAM)settings.bgColor);
			SendMessageW(hEdit, WM_SETFONT, (WPARAM)g_hCaptionFont, TRUE);
			SendMessageW(hEdit, EM_HIDESELECTION, TRUE, FALSE);
			SendMessageW(hEdit, WM_SETREDRAW, FALSE, 0);
			g_origEditProc = (WNDPROC)SetWindowLongPtrW(hEdit, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
			SetTimer(hWnd, IDT_POLL_CAPTION, POLL_INTERVAL_MS, nullptr);
		}
		g_hMainWnd = hWnd;
		g_hKbHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKbHook, nullptr, 0);
		HMENU hSysMenu = GetSystemMenu(hWnd, FALSE);
		if (hSysMenu) {
			DeleteMenu(hSysMenu, SC_MAXIMIZE, MF_BYCOMMAND);
			InsertMenuW(hSysMenu, SC_CLOSE, MF_BYCOMMAND | MF_STRING, IDM_SETTINGS, L"Settings");
		}
		if (settings.setTop) {
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		if (settings.setInvisible) {
			SetWindowDisplayAffinity(hWnd, WDA_EXCLUDEFROMCAPTURE);
		}
		{
			LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
			SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
			BYTE alpha = (BYTE)((settings.transparency * 255) / 100);
			SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
		}
		g_autoCopyHotkey   = settings.autoCopyHotkey;
		g_autoDeleteHotkey = settings.autoDeleteHotkey;
	}
	break;
	case WM_APP_FIND_AND_COPY:
		DoFindAndCopyWork();
		return 0;
	case WM_APP_CLEAR_HISTORY:
		DoClearHistory();
		return 0;
	case WM_APP_SETTINGS_CHANGED:
	{
		AppSettings settings = SettingsDialog::LoadSettings();
		if (g_hEditBrush) {
			DeleteObject(g_hEditBrush);
		}
		g_hEditBrush = CreateSolidBrush(settings.bgColor);
		if (g_hCaptionFont) {
			DeleteObject(g_hCaptionFont);
		}
		HDC hdc = GetDC(hWnd);
		int logPixels = hdc ? GetDeviceCaps(hdc, LOGPIXELSY) : 96;
		if (hdc) ReleaseDC(hWnd, hdc);
		g_hCaptionFont = CreateFontW(
			-MulDiv(settings.textSize, logPixels, 72),
			0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
			DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
		HWND hEdit = GetDlgItem(hWnd, IDC_CAPTION_EDIT);
		if (hEdit) {
			SendMessageW(hEdit, EM_SETBKGNDCOLOR, 0, (LPARAM)settings.bgColor);
			SendMessageW(hEdit, WM_SETFONT, (WPARAM)g_hCaptionFont, TRUE);
			ApplyYellowHighlight(hEdit);
			InvalidateRect(hEdit, nullptr, TRUE);
		}
		if (settings.setTop) {
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		} else {
			SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		if (settings.setInvisible) {
			SetWindowDisplayAffinity(hWnd, WDA_EXCLUDEFROMCAPTURE);
		} else {
			SetWindowDisplayAffinity(hWnd, WDA_NONE);
		}
		{
			LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
			if (!(exStyle & WS_EX_LAYERED)) {
				SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
			}
			BYTE alpha = (BYTE)((settings.transparency * 255) / 100);
			SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
		}
		g_autoCopyHotkey   = settings.autoCopyHotkey;
		g_autoDeleteHotkey = settings.autoDeleteHotkey;
		return 0;
	}
	case WM_SIZE:
	{
		HWND hEdit = GetDlgItem(hWnd, IDC_CAPTION_EDIT);
		if (hEdit) SetWindowPos(hEdit, nullptr, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER);
	}
	break;
	case WM_CTLCOLOREDIT:
	{
		AppSettings settings = SettingsDialog::LoadSettings();
		SetTextColor((HDC)wParam, settings.textColor);
		SetBkColor((HDC)wParam, settings.bgColor);
		return (LRESULT)g_hEditBrush;
	}
	case WM_TIMER:
		if (wParam == IDT_POLL_CAPTION) {
			std::wstring text = GetLiveCaptionText();
			if (text != g_lastCaptionText) {
				if (!text.empty()) {
					UpdateCaptionHistory(text);
				}
				g_lastCaptionText = std::move(text);
				HWND hEdit = GetDlgItem(hWnd, IDC_CAPTION_EDIT);
				if (hEdit) {
					SendMessageW(hEdit, WM_SETREDRAW, FALSE, 0);
					POINT ptScroll = {};
					if (g_userScrolledUp) {
						SendMessageW(hEdit, EM_GETSCROLLPOS, 0, (LPARAM)&ptScroll);
					}
					SetWindowTextW(hEdit, g_captionHistory.c_str());
					if (!g_anchorSetByUser) {
						g_anchorCharIndex = 0;
						g_anchorHistoryIndex = 0;
					}
					ApplyYellowHighlight(hEdit);
					if (g_userScrolledUp) {
						SendMessageW(hEdit, EM_SETSCROLLPOS, 0, (LPARAM)&ptScroll);
					}
					else {
						ScrollEditToBottom(hEdit);
					}
					SendMessageW(hEdit, WM_SETREDRAW, TRUE, 0);
					InvalidateRect(hEdit, nullptr, TRUE);
				}
			}
		}
		break;
	case WM_SYSCOMMAND:
		if (wParam == IDM_SETTINGS) {
			SettingsDialog::Show(hWnd);
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_SETTINGS_BUTTON) {
			MessageBoxW(hWnd, L"Settings panel will open here", L"Settings", MB_OK);
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
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
		if (g_hEditBrush) { DeleteObject(g_hEditBrush); g_hEditBrush = nullptr; }
		if (g_hCaptionFont) { DeleteObject(g_hCaptionFont); g_hCaptionFont = nullptr; }
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
