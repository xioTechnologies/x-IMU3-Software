// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#pragma once

#include "GraphWindow.h"
#include "OpenGL/Common/OpenGLRenderer.h"
#include "Ximu3.hpp"

class TemperatureGraphWindow : public GraphWindow
{
public:
    TemperatureGraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, OpenGLRenderer& openGLRenderer);

    ~TemperatureGraphWindow() override;

private:
    std::vector<uint64_t> callbackIds;
    std::function<void(ximu3::XIMU3_TemperatureMessage)> temperatureCallback;
};
