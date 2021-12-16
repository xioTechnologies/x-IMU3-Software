#pragma once

#include "../DevicePanel/DevicePanel.h"
#include "../OpenGL/GLRenderer.h"
#include "../OpenGL/Graph.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Window.h"
#include "Ximu3.hpp"

#define USE_GRAPH

class GraphWindow : public Window
#ifndef USE_GRAPH
    , private juce::Timer
#endif
{
public:
    GraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer);

    ~GraphWindow() override;

#ifndef USE_GRAPH

    void paint(juce::Graphics& g) override;

#endif

    void resized() override;

    void mouseDown(const juce::MouseEvent& mouseEvent) override;

#ifdef USE_GRAPH

    void mouseWheelMove(const juce::MouseEvent& mouseEvent, const juce::MouseWheelDetails& wheel) override;

#endif

private:
#ifdef USE_GRAPH
    std::unique_ptr<Graph> graph;
    juce::Point<int> lastMousePosition;
#else
    SimpleLabel yAxisLabel;
    std::vector<std::unique_ptr<SimpleLabel>> legendLabels, argumentLabels;
    SimpleLabel timestampLabel;

    std::vector<std::unique_ptr<std::atomic<float>>> arguments;
    std::atomic<uint64_t> timestamp { 0 };

    void timerCallback() override;

#endif
    Graph::Settings settings;
    std::atomic<bool> isPaused = false;

    std::function<void(ximu3::XIMU3_Statistics)> statisticsCallback;
    std::function<void(ximu3::XIMU3_InertialMessage)> inertialCallback;
    std::function<void(ximu3::XIMU3_MagnetometerMessage)> magnetometerCallback;
    std::function<void(ximu3::XIMU3_QuaternionMessage)> quaternionCallback;
    std::function<void(ximu3::XIMU3_RotationMatrixMessage)> rotationMatrixCallback;
    std::function<void(ximu3::XIMU3_EulerAnglesMessage)> eulerAnglesCallback;
    std::function<void(ximu3::XIMU3_LinearAccelerationMessage)> linearAccelerationCallback;
    std::function<void(ximu3::XIMU3_EarthAccelerationMessage)> earthAccelerationCallback;
    std::function<void(ximu3::XIMU3_HighGMessage)> highGCallback;
    std::function<void(ximu3::XIMU3_TemperatureMessage)> temperatureCallback;
    std::function<void(ximu3::XIMU3_BatteryMessage)> batteryCallback;
    std::function<void(ximu3::XIMU3_RssiMessage)> rssiCallback;
    std::function<void(ximu3::XIMU3_SerialMessage)> serialCallback; // TODO: Add graph window

    std::vector<uint64_t> callbackIDs;

    void update(const uint64_t timestamp_, const std::vector<float>& arguments_);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphWindow)
};
