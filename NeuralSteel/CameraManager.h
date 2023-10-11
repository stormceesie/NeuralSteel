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
	int delay;
	std::thread feedThread;
	HWND hMainWindow;
	std::mutex frameMutex;
	cv::Mat* frame;
	bool running;

	void CaptureFeed();
public:
	CameraManager(HWND hwnd, int cameraID = 0, int fps = 30);
	void start();
	void stop();
	cv::Mat GetLatestFrame();
	~CameraManager();
};