#pragma once
#include <windows.h>
#include <commctrl.h>
#include <vector>
#pragma comment(lib, "comctl32.lib")

class TabControl {
public:
	TabControl(HWND parentWnd, HINSTANCE hInst);
	~TabControl();

	void AddTab(LPCWSTR title);
	HWND GetTab(int index);
	void SelectTab(int index);
	void HandleTabChange();
	HWND GetHandle() const;

private:
	HWND hTab;
	HINSTANCE hInstance;
	std::vector<HWND> tabPages;
};