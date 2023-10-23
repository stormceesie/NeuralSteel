#pragma once
#include "pch.h"

namespace NeuralSteelNamespace
{
    class NeuralSteelPage : public winrt::Windows::UI::Xaml::Controls::PageT<NeuralSteelPage>
    {
    public:
        winrt::Windows::UI::Xaml::Controls::CaptureElement CameraPreview();
        ~NeuralSteelPage();
        NeuralSteelPage();
        winrt::Windows::Media::Capture::MediaCapture mediaCapture{ nullptr };
        void InitializeComponent();
        void StartCamera();
        void StopCamera();
        void OnMediaCaptureFailed(winrt::Windows::Media::Capture::MediaCapture const&, winrt::Windows::Media::Capture::MediaCaptureFailedEventArgs const& args);
    };
}
