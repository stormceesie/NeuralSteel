#pragma once
#include <windows.h>
#include <commctrl.h>
#include <vector>
#include "CameraManager.h"
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

class TabControl {
public:
	TabControl(HWND parentWnd, HINSTANCE hInst);
	~TabControl();

	void AddTab(LPCWSTR title, bool showCamera = false);
	HWND GetTab(int index);
	void HandleTabChange();
	HWND GetHandle() const;
	void UpdateCameraFeed(const winrt::Windows::Graphics::Imaging::SoftwareBitmap& softwareBitmap);
	bool GetCameraTabSelected();

private:
	HWND hTab;
	HWND hCameraFeed;
	HINSTANCE hInstance;
	HBITMAP hBitmap;
	BITMAPINFO bmi;
	HDC hMemDC;
	HDC hdcCameraFeed;
	std::vector<HWND> tabPages;
	bool isCameraTabSelected = true;
	int cameraTabIndex;
};