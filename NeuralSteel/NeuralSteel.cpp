#include "pch.h"

#include "NeuralSteel.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hMainWindow;
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
    std::wofstream logFile("logfile.txt");
    std::wstreambuf* originalCerrBuffer = std::wcerr.rdbuf();
    std::wcerr.rdbuf(logFile.rdbuf());
    MSG msg;
    try {

        loadingscreen.ShowLoadingScreen(hInstance);

        // Initialize global strings
        LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
        LoadStringW(hInstance, IDC_NEURALSTEEL, szWindowClass, MAX_LOADSTRING);
        MyRegisterClass(hInstance);

        // Perform application initialization:
        if (!InitInstance(hInstance, nCmdShow))
        {
            return FALSE;
        }

        winrt::init_apartment(winrt::apartment_type::single_threaded);
        winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource desktopSource;
        auto interop = desktopSource.as<IDesktopWindowXamlSourceNative>();
        HRESULT hr = interop->AttachToWindow(hMainWindow);
        winrt::check_hresult(hr);
        HWND hWndXamlIsland;
        hr = interop->get_WindowHandle(&hWndXamlIsland);
        winrt::check_hresult(hr);
        SetFocus(hWndXamlIsland);

        const auto newHeight = 300;
        const auto newWidth = 300;
        const auto margin = 5;
        SetWindowPos(hWndXamlIsland, 0, margin, margin, newHeight - margin, newWidth - margin, SWP_SHOWWINDOW);

        auto neuralSteelPage = winrt::make<NeuralSteelNamespace::NeuralSteelPage>();
        desktopSource.Content(neuralSteelPage);

        HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NEURALSTEEL));

        loadingscreen.DeleteLoadingScreen();

        // Main message loop:
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            BOOL xamlSourceProcessedMessage = FALSE;
            auto xamlSourceNative2 = desktopSource.as<IDesktopWindowXamlSourceNative2>();
            hr = xamlSourceNative2->PreTranslateMessage(&msg, &xamlSourceProcessedMessage);
            winrt::check_hresult(hr);
            if (!xamlSourceProcessedMessage && !TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    catch (const winrt::hresult_error& e) {
        OSVERSIONINFO osvi;
        ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        std::wcerr << L"WinRT Error: " << e.message().c_str() << L" (HRESULT: " << e.to_abi() << L")" << std::endl;
        std::wcerr << "Windows version: " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion << std::endl;
        std::wcerr << "Build number: " << osvi.dwBuildNumber << std::endl;
    }
    catch (const std::exception& e) {
        std::wcerr << L"Standard Exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::wcerr << L"Unknown exception cought." << std::endl;
    }
    std::wcerr.rdbuf(originalCerrBuffer);
    logFile.close();
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

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
    {
        // Windows 11 round window corners for better style.
        DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
        DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));
        break;
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
