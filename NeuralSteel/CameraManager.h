#pragma once
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>

class CameraManager {
private:
	cv::VideoCapture cap;
	int delay = 100;
	std::thread feedThread;
	HWND hMainWindow;
	std::mutex frameMutex;
	cv::Mat* frame = nullptr;
	bool running = false;

	void CaptureFeed();
public:
	CameraManager(HWND hwnd, int cameraID = 0, int fps = 30);
	void start();
	void stop();
	cv::Mat GetLatestFrame();
	~CameraManager();
};