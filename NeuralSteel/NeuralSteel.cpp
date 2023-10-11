// NeuralSteel.cpp : Defines the entry point for the application.
//
#include "TabControl.h"
#include <gdiplus.h>
#include "framework.h"
#include "NeuralSteel.h"
#pragma comment(lib, "gdiplus.lib")
#include <random>
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hLoadingWnd = NULL;
HWND hMainWindow;
TabControl* pTabControl = nullptr;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Returns all png files in folderpath (multiple loading screens is fancier)
std::vector<std::wstring> GetImageListFromFolder(const std::wstring& folderPath) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((folderPath + L"\\*png").c_str(), &findFileData);

    std::vector<std::wstring> files;

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            const std::wstring fileOrDirName = findFileData.cFileName;
            const std::wstring completeFileName = folderPath + L"\\" + fileOrDirName;

            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                files.push_back(completeFileName);
            }
        } while (FindNextFile(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
    return files;
}

// show loading screen png using direct use of windows API
void ShowLoadingScreen(HWND& hwnd, HINSTANCE hInstance) {
    // Window class voor het loading screen
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = DefWindowProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszClassName = L"LoadingClass";
    wcex.hbrBackground = NULL; // Geen achtergrond
    RegisterClassEx(&wcex);

    // Laden van de PNG met GDI+
    auto imageList = GetImageListFromFolder(L"LoadingScreens");
    if (imageList.empty()) return;

    // Random library to choose an random picture in the loadingscreens map
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<size_t> dist(0, imageList.size() - 1);
    Gdiplus::Image image(imageList[dist(mt)].c_str());

    // Create a loading screen based on the pixeldense of the height of the user his screen.
    float imageAspectRatio = static_cast<float>(image.GetWidth()) / image.GetHeight();
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int desiredHeight = static_cast<int>(screenWidth * 0.15); // 15% of the user his screen
    const int desiredWidth = static_cast<int>(desiredHeight * imageAspectRatio);

    // Create a loading window without any background
    hwnd = CreateWindowEx(WS_EX_LAYERED, L"LoadingClass", NULL, WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, desiredWidth, desiredHeight, nullptr, nullptr, hInstance, nullptr);

    // Maak compatibele DC's
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdcScreen, desiredWidth, desiredHeight);
    SelectObject(hdcMem, hbmMem);

    Gdiplus::Graphics graphics(hdcMem);
    graphics.DrawImage(&image, 0, 0, desiredWidth, desiredHeight);

    // Make shure to place the loading screen in the middle
    const int xPos = (screenWidth - desiredWidth) / 2;
    const int yPos = (screenHeight - desiredHeight) / 2;

    POINT ptPos = { xPos, yPos };
    SIZE sizeWnd = { desiredWidth, desiredHeight};
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    UpdateLayeredWindow(hwnd, hdcScreen, &ptPos, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    // Cleaning
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Temporary to test if the image is realy shown
    Sleep(5000);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 800, 400, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }
    hMainWindow = hWnd;
    ShowWindow(hWnd, SW_MAXIMIZE);
    UpdateWindow(hWnd);

    return TRUE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    InitCommonControls();
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&icex);

    ShowLoadingScreen(hLoadingWnd, hInstance);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NEURALSTEEL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    pTabControl = new TabControl(hMainWindow, hInstance);
    pTabControl->AddTab(L"Tab 1");
    pTabControl->AddTab(L"Tab 2");

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NEURALSTEEL));

    if (hLoadingWnd) {
        DestroyWindow(hLoadingWnd);
        hLoadingWnd = NULL;
    }

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
    delete pTabControl;
    Gdiplus::GdiplusShutdown(gdiplusToken);
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NEURALSTEEL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_NEURALSTEEL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
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
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_NOTIFY:
    {
        NMHDR* pNMHDR = reinterpret_cast<NMHDR*>(lParam);
        if (pNMHDR->hwndFrom == pTabControl->GetHandle() && pNMHDR->code == TCN_SELCHANGE)
        {
            int iSelectedTab = TabCtrl_GetCurSel(pTabControl->GetHandle());
            // Doe iets op basis van de geselecteerde tab (iSelectedTab).
        }
    }
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