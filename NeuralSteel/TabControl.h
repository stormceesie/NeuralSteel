#pragma once
#include <windows.h>
#include <commctrl.h>
#include <vector>
#include "CameraManager.h"
#pragma comment(lib, "comctl32.lib")

class TabControl {
public:
	TabControl(HWND parentWnd, HINSTANCE hInst);
	~TabControl();

	void AddTab(LPCWSTR title, bool showCamera = false);
	HWND GetTab(int index);
	void SelectTab(int index);
	void HandleTabChange();
	HWND GetHandle() const;
	void UpdateCameraFeed(const cv::Mat& frame);

private:
	HWND hTab;
	HWND hCameraFeed;
	HINSTANCE hInstance;
	HBITMAP hBitmap;
	BITMAPINFO bmi;
	HDC hMemDC;
	HDC hdcCameraFeed;
	std::vector<HWND> tabPages;
	bool isCameraTabSelected = false;
};