#include "CameraManager.h"

CameraManager::CameraManager(HWND hwnd, int cameraID, int fps) : hMainWindow(hwnd), delay(1000 / fps), running(false) {
	mediaCapture = winrt::Windows::Media::Capture::MediaCapture();
	winrt::Windows::Media::Capture::MediaCaptureInitializationSettings settings;
	settings.VideoDeviceId(std::to_wstring(cameraID));
	mediaCapture.InitializeAsync(settings).get();
}

void CameraManager::CaptureFeed() {
	while (running) {
		auto frame = mediaCapture.GetPreviewFrameAsync().get();
		latestFrame = frame.SoftwareBitmap();

		PostMessage(hMainWindow, WM_USER + 1, 0, 0);

		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}
}

void CameraManager::start() {
	if (!running) {
		running = true;
		feedThread = std::thread(&CameraManager::CaptureFeed, this);
	}
}

void CameraManager::stop() {
	running = false;
	if (feedThread.joinable()) {
		feedThread.join();
	}
}

winrt::Windows::Graphics::Imaging::SoftwareBitmap CameraManager::GetLatestFrame() {
	std::lock_guard<std::mutex> lock(frameMutex);
	return latestFrame;
}

CameraManager::~CameraManager() {
	stop();
	mediaCapture.Close();
}