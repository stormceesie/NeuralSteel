#include "TabControl.h"

TabControl::TabControl(HWND parentWnd, HINSTANCE hInst) {
	hInstance = hInst;
	hTab = CreateWindow(WC_TABCONTROL, L"", WS_CHILD | WS_VISIBLE, 0, 0, 800, 600, parentWnd, nullptr, hInstance, nullptr);
	SetBkColor(GetDC(hTab), RGB(23, 23, 23));
	ShowWindow(hTab, SW_SHOW);
	UpdateWindow(hTab);
}

TabControl::~TabControl() {

}

void TabControl::AddTab(LPCWSTR title) {
	TCITEM tie;
	tie.mask = TCIF_TEXT;
	tie.pszText = const_cast<LPWSTR>(title);
	TabCtrl_InsertItem(hTab, tabPages.size(), &tie);

	// Create new page
}

HWND TabControl::GetTab(int index) {
	if (index >= 0 && index < tabPages.size()) {
		return tabPages[index];
	}
	return nullptr;
}

void TabControl::SelectTab(int index) {
	if (index >= 0 && index < tabPages.size()) {
		TabCtrl_SetCurSel(hTab, index);

		for (int i = 0; i < tabPages.size(); ++i) {
			if (i == index) {
				ShowWindow(tabPages[i], SW_SHOW);
			}
			else {
				ShowWindow(tabPages[i], SW_HIDE);
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