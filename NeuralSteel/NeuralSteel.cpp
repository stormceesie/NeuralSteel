// NeuralSteel.cpp : Defines the entry point for the application.
//
#include "LoadingScreen.h"
#include "TabControl.h"
#include "NeuralSteel.h"
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hMainWindow;
TabControl* pTabControl = nullptr;
CameraManager* pCamera = nullptr;
LoadingScreen loadingscreen;
const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
const int screenWidth = GetSystemMetrics(SM_CXSCREEN);

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Returns all png files in folderpath (multiple loading screens is fancier)

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    const int windowHeight = static_cast<int>(screenHeight * 0.65);
    const int windowWidth = static_cast<int>(windowHeight * (16.0 / 9.0)); // acoording to standard 16/9 windows

    const int xPos = (screenWidth - windowWidth) / 2;
    const int yPos = (screenHeight - windowHeight) / 2;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        xPos, yPos, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    hMainWindow = hWnd;
    ShowWindow(hWnd, SW_MAXIMIZE);
    UpdateWindow(hWnd);

    return TRUE;
}

// ######################################### Main function ######################################################
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

    loadingscreen.ShowLoadingScreen(hInstance);

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
    pTabControl->AddTab(L"Camera", true);
    pTabControl->AddTab(L"Results");

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NEURALSTEEL));

    loadingscreen.DeleteLoadingScreen();

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


//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.

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

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
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
    case WM_USER + 1: {
        if (pTabControl && pTabControl->GetCameraTabSelected()) {
            pTabControl->UpdateCameraFeed(pCamera->GetLatestFrame());
        }
    }

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        pCamera->stop();
        delete pCamera;
        pCamera = nullptr;
        delete pTabControl;
        pTabControl = nullptr;
        PostQuitMessage(0);
        break;
    case WM_CREATE:
    {
        // Windows 11 round window corners for better style.
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));
        pCamera = new CameraManager(hWnd);
        pCamera->start();
        break;
    }
    case WM_NOTIFY:
    {
        NMHDR* pNMHDR = reinterpret_cast<NMHDR*>(lParam);
        if (pNMHDR->hwndFrom == pTabControl->GetHandle() && pNMHDR->code == TCN_SELCHANGE)
        {
            pTabControl->HandleTabChange();
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