#include "CameraManager.h"

// Capture a picture this is done for the amount of frames
void CameraManager::CaptureFeed() {
	while (running) {
		// Set new frame in frame pointer
		cap >> *frame;
		cv::cvtColor(*frame, *frame, cv::COLOR_BGR2RGB);

		// Send message to front thread to process the image in the GUI
		PostMessage(hMainWindow, WM_USER + 1, 0, 0);

		// wait till another picture needs to be taken
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
	}
}

// Constructor of CameraManager fps is set here
CameraManager::CameraManager(HWND hwnd, int cameraID, int fps) : hMainWindow(hwnd), cap(cameraID), delay(1000 / fps), running(false) {
	frame = new cv::Mat();
	if (!cap.isOpened()) {
		std::cerr << "Error: Camera could not be opened" << std::endl;
		exit(1);
	}
}

// Start the camera
void CameraManager::start() {
	if (!running) {
		running = true;
		// Camera is running on background thread
		feedThread = std::thread(&CameraManager::CaptureFeed, this);
	}
}

// Stop the camera process
void CameraManager::stop() {
	running = false;
	if (feedThread.joinable()) {
		feedThread.join();
	}
}

// Return the latest frame to the GUI
cv::Mat CameraManager::GetLatestFrame() {
	// Mutex for preventing race conditions
	std::lock_guard<std::mutex> lock(frameMutex);
	return *frame;
}

// Deconstructor of the CameraManager clean all pointers and stop camera process
CameraManager::~CameraManager() {
	stop();
	cap.release();
	if (frame) {
		delete frame;
	}
}