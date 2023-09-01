#pragma once

#include "../CommandMessage.h"
#include "../OpenGL/Common/GLRenderer.h"
#include <cassert>
#include "DevicePanelFooter.h"
#include "DevicePanelHeader.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <list>
#include "WindowContainer.h"
#include "Windows/Window.h"
#include "Ximu3.hpp"

class DevicePanelContainer;

class DevicePanel : public juce::Component,
                    private juce::AsyncUpdater
{
public:
    static constexpr int headerHeight = 26;
    static constexpr int footerHeight = 20;
    static constexpr int collapsedHeight = headerHeight + UILayout::panelMargin + footerHeight + UILayout::panelMargin;

    DevicePanel(const juce::ValueTree& windowLayout_,
                std::shared_ptr<ximu3::Connection> connection_,
                GLRenderer& glRenderer_,
                DevicePanelContainer& devicePanelContainer_,
                const juce::Colour& tag_);

    ~DevicePanel() override;

    void resized() override;

    std::shared_ptr<ximu3::Connection> getConnection();

    void sendCommands(const std::vector<CommandMessage>& commands, SafePointer <juce::Component> callbackOwner = nullptr, std::function<void(const std::vector<CommandMessage>& responses, const std::vector<CommandMessage>& failedCommands)> callback = nullptr);

    const juce::Colour& getTag() const;

    std::shared_ptr<Window> getOrCreateWindow(const juce::ValueTree& windowTree);

    void cleanupWindows();

    juce::String getTitle() const;

    DevicePanelContainer& getDevicePanelContainer();

private:
    const juce::ValueTree& windowLayout;
    std::shared_ptr<ximu3::Connection> connection;
    GLRenderer& glRenderer;
    DevicePanelContainer& devicePanelContainer;
    const juce::Colour tag;

    DevicePanelHeader header { *this, devicePanelContainer };
    DevicePanelFooter footer { *this };

    std::map<juce::Identifier, std::shared_ptr<Window>> windows;
    std::unique_ptr<WindowContainer> windowContainer;

    std::shared_ptr<std::atomic<bool>> destroyed = std::make_shared<std::atomic<bool>>(false);

    void connect();

    void handleAsyncUpdate() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DevicePanel)
};
