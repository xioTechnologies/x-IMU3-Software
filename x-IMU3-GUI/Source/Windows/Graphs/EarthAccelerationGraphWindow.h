// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#pragma once

#include "GraphWindow.h"
#include "OpenGL/Common/GLRenderer.h"
#include "Ximu3.hpp"

class EarthAccelerationGraphWindow : public GraphWindow
{
public:
    EarthAccelerationGraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer);

    ~EarthAccelerationGraphWindow() override;

private:
    std::vector<uint64_t> callbackIDs;
    std::function<void(ximu3::XIMU3_EarthAccelerationMessage)> earthAccelerationCallback;
};
