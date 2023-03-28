// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "GraphWindow.h"
#include "Ximu3.hpp"

class TemperatureGraphWindow : public GraphWindow
{
public:
    TemperatureGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer);

    ~TemperatureGraphWindow() override;

private:
    static Graph::Settings settings;

    std::vector<uint64_t> callbackIDs;
    std::function<void(ximu3::XIMU3_TemperatureMessage)> temperatureCallback;
};