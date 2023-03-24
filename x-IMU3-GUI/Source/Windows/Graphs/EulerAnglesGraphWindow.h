#include "GraphWindow.h"

class EulerAnglesGraphWindow : public GraphWindow
{
public:
    EulerAnglesGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer);

    ~EulerAnglesGraphWindow() override;

private:
    static Graph::Settings settings;

    std::vector<uint64_t> callbackIDs;
    std::function<void(ximu3::XIMU3_QuaternionMessage)> quaternionCallback;
    std::function<void(ximu3::XIMU3_RotationMatrixMessage)> rotationMatrixCallback;
    std::function<void(ximu3::XIMU3_EulerAnglesMessage)> eulerAnglesCallback;
    std::function<void(ximu3::XIMU3_LinearAccelerationMessage)> linearAccelerationCallback;
    std::function<void(ximu3::XIMU3_EarthAccelerationMessage)> earthAccelerationCallback;
};
