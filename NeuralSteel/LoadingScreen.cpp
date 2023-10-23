#include "pch.h"
#include "LoadingScreen.h"

LoadingScreen::LoadingScreen() {
    hLoadingWnd = NULL;
    ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
}

// Get all image locations from loading images
std::vector<std::wstring> LoadingScreen::GetImageListFromFolder(const std::wstring& folderPath) {
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

    // Return list of possible loading screens (empty when nothing is found)
    return files;
}

// Delete loading screen (When all loading is done loading screen can be destroyed)
void LoadingScreen::DeleteLoadingScreen() {
    if (hLoadingWnd) {
        DestroyWindow(hLoadingWnd);
        hLoadingWnd = NULL;
    }
}

// Show loading screen. this is a random picture from the LoadingScreens map png is supported
void LoadingScreen::ShowLoadingScreen(HINSTANCE hInstance) {
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

    // No images where found in the LoadingScreens map -> dont show any loading screen return!
    if (imageList.empty()) return;

    // Random library to choose an random picture in the loadingscreens map
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<size_t> dist(0, imageList.size() - 1);

    // Load loading screen into RAM
    Gdiplus::Image image(imageList[dist(mt)].c_str());

    // Create a loading screen based on the pixeldense of the height of the user his screen.
    float imageAspectRatio = static_cast<float>(image.GetWidth()) / image.GetHeight();
    const int desiredHeight = static_cast<int>(ScreenHeight * 0.30); // 30% of the user his screen
    const int desiredWidth = static_cast<int>(desiredHeight * imageAspectRatio);

    // Create a loading window supporting png images
    hLoadingWnd = CreateWindowEx(WS_EX_LAYERED, L"LoadingClass", NULL, WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, desiredWidth, desiredHeight, nullptr, nullptr, hInstance, nullptr);

    // Maak compatibele DC's
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdcScreen, desiredWidth, desiredHeight);
    SelectObject(hdcMem, hbmMem);

    // Draw the PNG image 
    Gdiplus::Graphics graphics(hdcMem);
    graphics.DrawImage(&image, 0, 0, desiredWidth, desiredHeight);

    // Make shure to place the loading screen in the middle of the screen (based on pixel density of user)
    const int xPos = (ScreenWidth - desiredWidth) / 2;
    const int yPos = (ScreenHeight - desiredHeight) / 2;
    POINT ptPos = { xPos, yPos };
    SIZE sizeWnd = { desiredWidth, desiredHeight };
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    UpdateLayeredWindow(hLoadingWnd, hdcScreen, &ptPos, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    // Cleaning
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    // Everything is ready -> show the loading screen
    ShowWindow(hLoadingWnd, SW_SHOW);
    UpdateWindow(hLoadingWnd);

    // Temp sleep for loading screen inspection (can be removed)
    Sleep(2000);
}

LoadingScreen::~LoadingScreen() {
    // Deconstruct
}
