#include "../DevicePanelContainer.h"
#include "ApplicationSettings.h"
#include "DevicePanel.h"
#include "Windows/DeviceSettingsWindow.h"
// Start of code block #0 generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py
#include "Windows/Graphs/GyroscopeGraphWindow.h"
#include "Windows/Graphs/AccelerometerGraphWindow.h"
#include "Windows/Graphs/MagnetometerGraphWindow.h"
#include "Windows/Graphs/EulerAnglesGraphWindow.h"
#include "Windows/Graphs/LinearAccelerationGraphWindow.h"
#include "Windows/Graphs/EarthAccelerationGraphWindow.h"
#include "Windows/Graphs/HighGAccelerometerGraphWindow.h"
#include "Windows/Graphs/TemperatureGraphWindow.h"
#include "Windows/Graphs/BatteryPercentageGraphWindow.h"
#include "Windows/Graphs/BatteryVoltageGraphWindow.h"
#include "Windows/Graphs/RssiPercentageGraphWindow.h"
#include "Windows/Graphs/RssiPowerGraphWindow.h"
#include "Windows/Graphs/ReceivedMessageRateGraphWindow.h"
#include "Windows/Graphs/ReceivedDataRateGraphWindow.h"
// End of code block #0 generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py
#include "Windows/Graphs/GraphWindow.h"
#include "Windows/SerialAccessoryTerminalWindow.h"
#include "Windows/ThreeDViewWindow.h"
#include "Windows/WindowIDs.h"

DevicePanel::DevicePanel(const juce::ValueTree& windowLayout_,
                         std::shared_ptr<ximu3::Connection> connection_,
                         GLRenderer& glRenderer_,
                         DevicePanelContainer& devicePanelContainer_,
                         const juce::Colour& colourTag_)
        : windowLayout(windowLayout_),
          connection(connection_),
          glRenderer(glRenderer_),
          devicePanelContainer(devicePanelContainer_),
          colourTag(colourTag_)
{
    windowContainer = std::make_unique<WindowContainer>(*this, windowLayout);
    addAndMakeVisible(*windowContainer);

    addAndMakeVisible(header);
    addAndMakeVisible(footer);
}

DevicePanel::~DevicePanel()
{
    connection->close();
}

void DevicePanel::resized()
{
    auto bounds = getLocalBounds();

    header.setBounds(bounds.removeFromTop(headerHeight + UILayout::panelMargin));
    footer.setBounds(bounds.removeFromBottom(footerHeight + UILayout::panelMargin));

    bounds.removeFromTop(UILayout::panelMargin);
    windowContainer->setBounds(bounds);
}

ximu3::Connection& DevicePanel::getConnection()
{
    return *connection;
}

void DevicePanel::sendCommands(const std::vector<CommandMessage>& commands, SafePointer <juce::Component> callbackOwner, std::function<void(const std::vector<CommandMessage>& responses, const std::vector<CommandMessage>& failedCommands)> callback)
{
    connection->sendCommandsAsync({ commands.begin(), commands.end() }, ApplicationSettings::getSingleton().retries, ApplicationSettings::getSingleton().timeout, [this, commands = commands, callbackOwner, callback](auto responses_)
    {
        const std::vector<CommandMessage> responses(responses_.begin(), responses_.end());

        std::vector<CommandMessage> failedCommands;
        for (const auto& command : commands)
        {
            if (std::find(responses.begin(), responses.end(), command) == responses.end())
            {
                failedCommands.push_back(command);
            }
        }

        juce::MessageManager::callAsync([&, callbackOwner, callback, responses, failedCommands]
                                        {
                                            header.updateDeviceDescriptor(responses);

                                            if (callbackOwner != nullptr && callback != nullptr)
                                            {
                                                callback(responses, failedCommands);
                                            }
                                        });
    });
}

const juce::Colour& DevicePanel::getColourTag() const
{
    return colourTag;
}

std::shared_ptr<Window> DevicePanel::getOrCreateWindow(const juce::ValueTree& windowTree)
{
    const auto type = windowTree.getType();
    auto& window = windows[type];
    if (window != nullptr)
    {
        return window;
    }
    if (type == WindowIDs::DeviceSettings)
    {
        return window = std::make_shared<DeviceSettingsWindow>(windowLayout, windowTree.getType(), *this);
    }
    if (type == WindowIDs::SerialAccessoryTerminal)
    {
        return window = std::make_shared<SerialAccessoryTerminalWindow>(windowLayout, windowTree.getType(), *this);
    }

    if (type == WindowIDs::ThreeDView)
    {
        return window = std::make_shared<ThreeDViewWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }

    // Start of code block #1 generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py
    if (type == WindowIDs::Gyroscope)
    {
        return window = std::make_shared<GyroscopeGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::Accelerometer)
    {
        return window = std::make_shared<AccelerometerGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::Magnetometer)
    {
        return window = std::make_shared<MagnetometerGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::EulerAngles)
    {
        return window = std::make_shared<EulerAnglesGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::LinearAcceleration)
    {
        return window = std::make_shared<LinearAccelerationGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::EarthAcceleration)
    {
        return window = std::make_shared<EarthAccelerationGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::HighGAccelerometer)
    {
        return window = std::make_shared<HighGAccelerometerGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::Temperature)
    {
        return window = std::make_shared<TemperatureGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::BatteryPercentage)
    {
        return window = std::make_shared<BatteryPercentageGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::BatteryVoltage)
    {
        return window = std::make_shared<BatteryVoltageGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::RssiPercentage)
    {
        return window = std::make_shared<RssiPercentageGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::RssiPower)
    {
        return window = std::make_shared<RssiPowerGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::ReceivedMessageRate)
    {
        return window = std::make_shared<ReceivedMessageRateGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    if (type == WindowIDs::ReceivedDataRate)
    {
        return window = std::make_shared<ReceivedDataRateGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
    }
    // End of code block #1 generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

    return nullptr;
}

void DevicePanel::cleanupWindows()
{
    triggerAsyncUpdate();
}

juce::String DevicePanel::getDeviceDescriptor() const
{
    return header.getDeviceDescriptor();
}

DevicePanelContainer& DevicePanel::getDevicePanelContainer()
{
    return devicePanelContainer;
}

void DevicePanel::handleAsyncUpdate()
{
    for (auto& window : windows)
    {
        // Destroy if the use_count is 1, since it means no one is pointing to it except us
        if (window.second.use_count() == 1)
        {
            window.second.reset();
        }
    }
}
