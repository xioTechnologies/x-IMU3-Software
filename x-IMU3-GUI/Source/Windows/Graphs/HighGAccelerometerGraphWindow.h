// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#pragma once

#include "GraphWindow.h"
#include "OpenGL/Common/GLRenderer.h"
#include "Ximu3.hpp"

class HighGAccelerometerGraphWindow : public GraphWindow
{
public:
    HighGAccelerometerGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, DevicePanel& devicePanel_, GLRenderer& glRenderer);

    ~HighGAccelerometerGraphWindow() override;

private:
    static juce::ValueTree settingsTree_;

    std::vector<uint64_t> callbackIDs;
    std::function<void(ximu3::XIMU3_HighGAccelerometerMessage)> highGAccelerometerCallback;
};
