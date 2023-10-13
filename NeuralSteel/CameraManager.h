#pragma once
#include <windows.h>
#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <winrt/windows.media.capture.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Foundation.h>

class CameraManager {
private:
	HWND hMainWindow;
	winrt::Windows::Media::Capture::MediaCapture mediaCapture{ nullptr };
	int delay;
	bool running;
	std::thread feedThread;
	winrt::Windows::Graphics::Imaging::SoftwareBitmap latestFrame{ nullptr };
	std::mutex frameMutex;

public:
	CameraManager(HWND hwnd, int cameraID, int fps);
	void CaptureFeed();
	void start();
	void stop();
	winrt::Windows::Graphics::Imaging::SoftwareBitmap GetLatestFrame();
	~CameraManager();


};