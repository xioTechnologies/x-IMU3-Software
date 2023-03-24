#include "GraphWindow.h"

class GyroscopeGraphWindow : public GraphWindow
{
public:
    GyroscopeGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer);

    ~GyroscopeGraphWindow() override;

private:
    static Graph::Settings settings;

    std::vector<uint64_t> callbackIDs;
    std::function<void(ximu3::XIMU3_InertialMessage)> inertialCallback;
};
