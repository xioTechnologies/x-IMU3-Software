#include "ApplicationSettings.h"
#include "ConnectionPanel.h"
#include "ConnectionPanelContainer.h"
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
#include "Windows/Graphs/SerialAccessoryCsvsGraphWindow.h"
#include "Windows/Graphs/ReceivedMessageRateGraphWindow.h"
#include "Windows/Graphs/ReceivedDataRateGraphWindow.h"
// End of code block #0 generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py
#include "Windows/SerialAccessoryTerminalWindow.h"
#include "Windows/ThreeDViewWindow.h"
#include "Windows/WindowIDs.h"

ConnectionPanel::ConnectionPanel(const juce::ValueTree& windowLayout_,
                                 std::shared_ptr<ximu3::Connection> connection_,
                                 GLRenderer& glRenderer_,
                                 juce::ThreadPool& threadPool_,
                                 ConnectionPanelContainer& connectionPanelContainer_,
                                 const juce::Colour& tag_)
    : windowLayout(windowLayout_),
      connection(connection_),
      glRenderer(glRenderer_),
      threadPool(threadPool_),
      connectionPanelContainer(connectionPanelContainer_),
      tag(tag_)
{
    addAndMakeVisible(header);
    addAndMakeVisible(footer);
    addChildComponent(disabledOverlay);

    header.onRetry = [&]
    {
        connect();
    };

    connect();
}

ConnectionPanel::~ConnectionPanel()
{
    connection->close();

    *destroyed = true;
}

void ConnectionPanel::resized()
{
    auto bounds = getLocalBounds();
    disabledOverlay.setBounds(bounds);

    header.setBounds(bounds.removeFromTop(headerHeight + UILayout::panelMargin));
    footer.setBounds(bounds.removeFromBottom(footerHeight + UILayout::panelMargin));

    if (windowContainer)
    {
        bounds.removeFromTop(UILayout::panelMargin);
        windowContainer->setBounds(bounds);
    }
}

std::shared_ptr<ximu3::Connection> ConnectionPanel::getConnection()
{
    return connection;
}

void ConnectionPanel::sendCommands(const std::vector<CommandMessage>& commands, SafePointer<juce::Component> callbackOwner, std::function<void(const std::vector<CommandMessage>& responses)> callback)
{
    connection->sendCommandsAsync({ commands.begin(), commands.end() }, ApplicationSettings::getSingleton().commands.retries, ApplicationSettings::getSingleton().commands.timeout, [&, callbackOwner, callback](auto responses)
    {
        juce::MessageManager::callAsync([&, callbackOwner, callback, responses]
        {
            header.updateTitle({ responses.begin(), responses.end() });

            if (callbackOwner != nullptr && callback != nullptr)
            {
                callback({ responses.begin(), responses.end() });
            }
        });
    });
}

const juce::Colour& ConnectionPanel::getTag() const
{
    return tag;
}

std::shared_ptr<Window> ConnectionPanel::getOrCreateWindow(const juce::ValueTree& windowTree)
{
    const auto type = windowTree.getType();
    auto& window = windows[type];
    if (window != nullptr)
    {
        return window;
    }
    if (type == WindowIDs::DeviceSettings)
    {
        return window = std::make_shared<DeviceSettingsWindow>(windowLayout, type, *this);
    }
    if (type == WindowIDs::ThreeDView)
    {
        return window = std::make_shared<ThreeDViewWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::SerialAccessoryTerminal)
    {
        return window = std::make_shared<SerialAccessoryTerminalWindow>(windowLayout, type, *this);
    }
    // Start of code block #1 generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py
    if (type == WindowIDs::Gyroscope)
    {
        return window = std::make_shared<GyroscopeGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::Accelerometer)
    {
        return window = std::make_shared<AccelerometerGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::Magnetometer)
    {
        return window = std::make_shared<MagnetometerGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::EulerAngles)
    {
        return window = std::make_shared<EulerAnglesGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::LinearAcceleration)
    {
        return window = std::make_shared<LinearAccelerationGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::EarthAcceleration)
    {
        return window = std::make_shared<EarthAccelerationGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::HighGAccelerometer)
    {
        return window = std::make_shared<HighGAccelerometerGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::Temperature)
    {
        return window = std::make_shared<TemperatureGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::BatteryPercentage)
    {
        return window = std::make_shared<BatteryPercentageGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::BatteryVoltage)
    {
        return window = std::make_shared<BatteryVoltageGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::RssiPercentage)
    {
        return window = std::make_shared<RssiPercentageGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::RssiPower)
    {
        return window = std::make_shared<RssiPowerGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::SerialAccessoryCsvs)
    {
        return window = std::make_shared<SerialAccessoryCsvsGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::ReceivedMessageRate)
    {
        return window = std::make_shared<ReceivedMessageRateGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    if (type == WindowIDs::ReceivedDataRate)
    {
        return window = std::make_shared<ReceivedDataRateGraphWindow>(windowLayout, type, *this, glRenderer);
    }
    // End of code block #1 generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

    jassertfalse; // type not in WindowIDs
    return window = std::make_shared<Window>(windowLayout, type, *this, "");
}

void ConnectionPanel::cleanupWindows()
{
    triggerAsyncUpdate();
}

juce::String ConnectionPanel::getTitle() const
{
    return header.getTitle();
}

ConnectionPanelContainer& ConnectionPanel::getConnectionPanelContainer()
{
    return connectionPanelContainer;
}

void ConnectionPanel::setOverlayVisible(const bool visible)
{
    disabledOverlay.setVisible(visible);
}

void ConnectionPanel::connect()
{
    header.setState(ConnectionPanelHeader::State::connecting);

    connection->openAsync([&, destroyed_ = destroyed](auto result)
    {
        juce::MessageManager::callAsync([&, destroyed_, result]
        {
            if (*destroyed_)
            {
                return;
            }

            if (result != ximu3::XIMU3_ResultOk)
            {
                header.setState(ConnectionPanelHeader::State::connectionFailed);
                return;
            }

            windowContainer = std::make_unique<WindowContainer>(*this, windowLayout);
            addAndMakeVisible(*windowContainer);
            resized();

            header.setState(ConnectionPanelHeader::State::connected);
        });
    });
}

void ConnectionPanel::handleAsyncUpdate()
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
