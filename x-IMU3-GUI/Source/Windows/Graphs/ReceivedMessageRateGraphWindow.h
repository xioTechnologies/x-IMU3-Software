#include "GraphWindow.h"

class ReceivedMessageRateGraphWindow : public GraphWindow
{
public:
    ReceivedMessageRateGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer);

    ~ReceivedMessageRateGraphWindow() override;

private:
    static Graph::Settings settings;

    std::vector<uint64_t> callbackIDs;
    std::function<void(ximu3::XIMU3_Statistics)> statisticsCallback;
};
