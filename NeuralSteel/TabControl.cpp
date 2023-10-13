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

void TabControl::UpdateCameraFeed(const winrt::Windows::Graphics::Imaging::SoftwareBitmap& softwareBitmap) {
	RECT tabRect;
	GetClientRect(hTab, &tabRect);

	// ... [Behoud de rest van de code die de marges, doelgrootte, etc. berekent]

	// Zorg ervoor dat de SoftwareBitmap in BGRA8-formaat is
	auto convertedBitmap = winrt::Windows::Graphics::Imaging::SoftwareBitmap::Convert(softwareBitmap, winrt::Windows::Graphics::Imaging::BitmapPixelFormat::Bgra8);
	auto buffer = convertedBitmap.LockBuffer(winrt::Windows::Graphics::Imaging::BitmapBufferAccessMode::Read);
	auto reference = buffer.CreateReference();

	BYTE* sourcePixels = nullptr;
	uint32_t capacity = 0;
	winrt::check_hresult(reinterpret_cast<::Windows::Storage::Streams::IBufferByteAccess*>(reference.as<::Windows::Storage::Streams::IBuffer>().get())->Buffer(&sourcePixels));
	winrt::check_hresult(reference.as<::Windows::Storage::Streams::IBuffer>()->get_Capacity(&capacity));

	// Update BITMAPINFO for the new frame dimensions, etc.
	// ...

	// Update hBitmap with the pixel data from the SoftwareBitmap
	SetDIBits(hdcCameraFeed, hBitmap, 0, convertedBitmap.PixelHeight(), sourcePixels, &bmi, DIB_RGB_COLORS);

	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	// Gebruik StretchBlt om het gehele beeld te schalen naar het doelgebied
	StretchBlt(hdcCameraFeed, 0, 0, targetWidth, targetHeight, hMemDC, 0, 0, convertedBitmap.PixelWidth(), convertedBitmap.PixelHeight(), SRCCOPY);

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