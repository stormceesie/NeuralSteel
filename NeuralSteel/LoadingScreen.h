#pragma once
#include "framework.h"

class LoadingScreen {
public:
	void ShowLoadingScreen(HINSTANCE hInstance);
	void DeleteLoadingScreen();
	LoadingScreen();
	~LoadingScreen();
private:
	std::vector<std::wstring> GetImageListFromFolder(const std::wstring& folderPath);
	HWND hLoadingWnd;
	int ScreenWidth;
	int ScreenHeight;
};