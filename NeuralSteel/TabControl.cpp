#include "TabControl.h"

TabControl::TabControl(HWND parentWnd, HINSTANCE hInst) {
	hInstance = hInst;
	hMemDC = CreateCompatibleDC(GetDC(hCameraFeed));  // Direct gebruik van GetDC() hier
	hTab = CreateWindow(WC_TABCONTROL, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TCS_FLATBUTTONS, 0, 0, 800, 600, parentWnd, nullptr, hInstance, nullptr);
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
	// Controleer of hBitmap bestaat en of de afmetingen correct zijn, zo niet, maak een nieuwe
	if (!hBitmap || bmi.bmiHeader.biWidth != frame.cols || abs(bmi.bmiHeader.biHeight) != frame.rows) {
		// Initialiseer de BITMAPINFO structuur hier dynamisch
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = frame.cols;
		bmi.bmiHeader.biHeight = -frame.rows;  // Negatieve waarde voor top-down bitmap
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 24;
		if (hBitmap) {
			DeleteObject(hBitmap);
		}
		hBitmap = CreateCompatibleBitmap(hdcCameraFeed, bmi.bmiHeader.biWidth, abs(bmi.bmiHeader.biHeight));
	}

	SetDIBits(hdcCameraFeed, hBitmap, 0, frame.rows, frame.data, &bmi, DIB_RGB_COLORS);

	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	BitBlt(hdcCameraFeed, 0, 0, frame.cols, frame.rows, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOldBitmap);
}

void TabControl::SelectTab(int index) {
	if (index >= 0 && index < tabPages.size()) {
		TabCtrl_SetCurSel(hTab, index);

		for (int i = 0; i < tabPages.size(); ++i) {
			if (i == index) {
				ShowWindow(tabPages[i], SW_SHOW);

				// Als de geselecteerde tab de camera-tab is, toon dan de camera feed
				if (isCameraTabSelected && hCameraFeed) {
					ShowWindow(hCameraFeed, SW_SHOW);
				}
			}
			else {
				ShowWindow(tabPages[i], SW_HIDE);

				// Als de huidige tab niet de camera-tab is, verberg dan de camera feed
				if (isCameraTabSelected && hCameraFeed) {
					ShowWindow(hCameraFeed, SW_HIDE);
				}
			}
		}
	}
}

void TabControl::HandleTabChange() {
	int iPage = TabCtrl_GetCurSel(hTab);
}

HWND TabControl::GetHandle() const {
	return hTab;
}