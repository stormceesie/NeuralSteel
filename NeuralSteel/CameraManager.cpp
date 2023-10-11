#include "CameraManager.h"

void CameraManager::CaptureFeed() {
	while (running) {
		cv::Mat frame;
		cap >> frame;

		if (frame.empty()) {
			std::cerr << "Error: received empty frame!" << std::endl;
			break;
		}

		{
			std::lock_guard<std::mutex> lock(frameMutex);
			latestFrame = frame.clone(); // Safe frame in shared buffer.
		}

		PostMessage(hMainWindow, WM_USER + 1, 0, 0);
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}
}

CameraManager::CameraManager(HWND hwnd, int cameraID, int fps) : hMainWindow(hwnd), cap(cameraID), delay(1000 / fps), running(false) {
	if (!cap.isOpened()) {
		std::cerr << "Error: Camera could not be opened" << std::endl;
		exit(1);
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

cv::Mat CameraManager::GetLatestFrame() {
	std::lock_guard<std::mutex> lock(frameMutex);
	return latestFrame.clone();
}


CameraManager::~CameraManager() {
	stop();
	cap.release();
}
