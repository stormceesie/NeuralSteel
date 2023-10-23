#pragma once

#include "pch.h"
#include "NeuralSteelPage.xaml.h"

NeuralSteelNamespace::NeuralSteelPage::NeuralSteelPage()
{
    InitializeComponent();
}

NeuralSteelNamespace::NeuralSteelPage::~NeuralSteelPage()
{
    StopCamera();
}

void NeuralSteelNamespace::NeuralSteelPage::InitializeComponent() {
    try {
        mediaCapture = winrt::Windows::Media::Capture::MediaCapture();
        mediaCapture.Failed({ this, &NeuralSteelPage::OnMediaCaptureFailed });
        StartCamera();
    }
    catch (const winrt::hresult_error& ex) {
        std::wcerr << ex.message().c_str() << std::endl;
    }
}

void NeuralSteelNamespace::NeuralSteelPage::StartCamera() {
    try {
        auto settings = winrt::Windows::Media::Capture::MediaCaptureInitializationSettings();
        settings.StreamingCaptureMode(winrt::Windows::Media::Capture::StreamingCaptureMode::Video);

        // Asynchrone camera-initialisatie
        mediaCapture.InitializeAsync(settings).Completed([this](auto&&, auto status)
            {
                if (status == winrt::Windows::Foundation::AsyncStatus::Completed)
                {
                    auto cameraPreview = this->CameraPreview();
                    cameraPreview.Source(mediaCapture);
                    mediaCapture.StartPreviewAsync();
                }
                else
                {
                    std::wcerr << "Failed starting camera" << std::endl;
                }
            });
    }
    catch (const winrt::hresult_error& ex) {
        std::wcerr << "Failed starting camera" << std::endl;
    }
}

void NeuralSteelNamespace::NeuralSteelPage::StopCamera() {
    if (mediaCapture) {
        mediaCapture.StopPreviewAsync();
        mediaCapture.Close();
        mediaCapture = nullptr;
    }
}

void NeuralSteelNamespace::NeuralSteelPage::OnMediaCaptureFailed(winrt::Windows::Media::Capture::MediaCapture const&, winrt::Windows::Media::Capture::MediaCaptureFailedEventArgs const& args) {
    std::wcerr << "Media capture failed with error: " << args.Message().c_str() << std::endl;
    StopCamera();
}

winrt::Windows::UI::Xaml::Controls::CaptureElement NeuralSteelNamespace::NeuralSteelPage::CameraPreview()
{
    try {
        return this->FindName(L"cameraPreview").as<winrt::Windows::UI::Xaml::Controls::CaptureElement>();
    }
    catch (const winrt::hresult_error& ex) {
        std::wcerr << ex.message().c_str() << std::endl;
    }
}
