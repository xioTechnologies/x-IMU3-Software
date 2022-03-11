#include "../DevicePanelContainer.h"
#include "ApplicationSettings.h"
#include "DevicePanel.h"
#include "Windows/DeviceSettingsWindow.h"
#include "Windows/GraphWindow.h"
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

    return window = std::make_shared<GraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);
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
