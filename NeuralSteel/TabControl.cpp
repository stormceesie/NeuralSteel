#include "TabControl.h"

TabControl::TabControl(HWND parentWnd, HINSTANCE hInst) {
	int tabHeight = static_cast<int>(GetSystemMetrics(SM_CYSCREEN) * 0.8);
	int tabWidth = (16 * tabHeight) / 9;
	hInstance = hInst;
	hMemDC = CreateCompatibleDC(GetDC(hCameraFeed));  // Direct gebruik van GetDC() hier
	hTab = CreateWindow(WC_TABCONTROL, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TCS_FLATBUTTONS, 0, 0, tabWidth, tabHeight, parentWnd, nullptr, hInstance, nullptr);
	SetBkColor(GetDC(hTab), RGB(150, 0, 0));
	ShowWindow(hTab, SW_SHOW);
	UpdateWindow(hTab);
}

TabControl::~TabControl() {
	DeleteDC(hMemDC);
	if (hdcCameraFeed) {
		ReleaseDC(hCameraFeed, hdcCameraFeed);
	}
	if (hBitmap) {
		DeleteObject(hBitmap);
	}
}

void TabControl::AddTab(LPCWSTR title, bool showCamera) {
	TCITEM tie;
	tie.mask = TCIF_TEXT;
	tie.pszText = const_cast<LPWSTR>(title);
	int tabIndex = TabCtrl_InsertItem(hTab, tabPages.size(), &tie);

	if (showCamera) {
		cameraTabIndex = tabIndex;
		isCameraTabSelected = true;
		RECT tabRect;
		TabCtrl_GetItemRect(hTab, tabIndex, &tabRect);
		hCameraFeed = CreateWindow(L"STATIC", nullptr, WS_CHILD | WS_VISIBLE,
			tabRect.left + 10, tabRect.top + 30,
			780, 560,
			hTab, nullptr, hInstance, nullptr);
		SetWindowLongPtr(hCameraFeed, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		hdcCameraFeed = GetDC(hCameraFeed);  // Bewaar de DC voor hCameraFeed
	}
}

HWND TabControl::GetTab(int index) {
	if (index >= 0 && index < tabPages.size()) {
		return tabPages[index];
	}
	return nullptr;
}

void TabControl::UpdateCameraFeed(const cv::Mat& frame) {
	RECT tabRect;
	GetClientRect(hTab, &tabRect);

	// Bepaal de marges gebaseerd op de gegeven percentages
	int topMargin = static_cast<int>((tabRect.bottom - tabRect.top) * 0.05);
	int otherMargin = static_cast<int>((tabRect.bottom - tabRect.top) * 0.02);

	float aspectRatio = static_cast<float>(frame.cols) / frame.rows;

	// Bepaal de doelgrootte en positie
	int targetHeight = tabRect.bottom - tabRect.top - topMargin - otherMargin;
	int targetWidth = static_cast<int>(targetHeight * aspectRatio);

	int leftMargin = (tabRect.right - targetWidth) / 2; // Centreer het beeld horizontaal

	SetWindowPos(hCameraFeed, NULL, leftMargin, topMargin, targetWidth, targetHeight, SWP_NOZORDER);

	if (!hBitmap || bmi.bmiHeader.biWidth != frame.cols || abs(bmi.bmiHeader.biHeight) != frame.rows) {
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = frame.cols;
		bmi.bmiHeader.biHeight = -frame.rows;  // Negatieve waarde voor top-down bitmap
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 24;
		if (hBitmap) {
			DeleteObject(hBitmap);
		}
		hBitmap = CreateCompatibleBitmap(hdcCameraFeed, frame.cols, frame.rows);
	}

	SetDIBits(hdcCameraFeed, hBitmap, 0, frame.rows, frame.data, &bmi, DIB_RGB_COLORS);

	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	// Gebruik StretchBlt om het gehele beeld te schalen naar het doelgebied
	StretchBlt(hdcCameraFeed, 0, 0, targetWidth, targetHeight, hMemDC, 0, 0, frame.cols, frame.rows, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
}

HWND TabControl::GetHandle() const {
	return hTab;
}

bool TabControl::GetCameraTabSelected() {
	return isCameraTabSelected;
}

void TabControl::HandleTabChange() {
	int iPage = TabCtrl_GetCurSel(hTab);

	// Verberg de huidige camerafeed
	if (hCameraFeed) {
		ShowWindow(hCameraFeed, SW_HIDE);
	}

	if (iPage == cameraTabIndex) {
		isCameraTabSelected = true;
		if (hCameraFeed) {
			ShowWindow(hCameraFeed, SW_SHOW);
		}
	}
	else {
		isCameraTabSelected = false;
	}
}