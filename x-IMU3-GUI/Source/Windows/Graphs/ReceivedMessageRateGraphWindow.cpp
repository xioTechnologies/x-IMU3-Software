#include "ReceivedMessageRateGraphWindow.h"

Graph::Settings ReceivedMessageRateGraphWindow::settings = []
{
    Graph::Settings settings_;
    settings_.horizontal.autoscale = true;
    return settings_;
}();

ReceivedMessageRateGraphWindow::ReceivedMessageRateGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type, devicePanel_, glRenderer, "Receive rate (messages/s)", {{{}, juce::Colours::yellow }}, settings)
{
    callbackIDs = { devicePanel.getConnection().addStatisticsCallback(statisticsCallback = [&](auto message)
    {
        update(message.timestamp, { (float) message.message_rate });
    }) };
}

ReceivedMessageRateGraphWindow::~ReceivedMessageRateGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection().removeCallback(callbackID);
    }
}
