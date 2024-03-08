#pragma once

#include <cassert>
#include "CommandMessage.h"
#include "ConnectionPanelFooter.h"
#include "ConnectionPanelHeader.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <list>
#include "OpenGL/Common/GLRenderer.h"
#include "WindowContainer.h"
#include "Windows/Window.h"
#include "Ximu3.hpp"

class ConnectionPanelContainer;

class ConnectionPanel : public juce::Component,
                        private juce::AsyncUpdater
{
public:
    static constexpr int headerHeight = 26;
    static constexpr int footerHeight = 20;
    static constexpr int collapsedHeight = headerHeight + UILayout::panelMargin + footerHeight + UILayout::panelMargin;

    ConnectionPanel(const juce::ValueTree& windowLayout_,
                    std::shared_ptr<ximu3::Connection> connection_,
                    GLRenderer& glRenderer_,
                    juce::ThreadPool& threadPool_,
                    ConnectionPanelContainer& connectionPanelContainer_,
                    const juce::Colour& tag_);

    ~ConnectionPanel() override;

    void resized() override;

    std::shared_ptr<ximu3::Connection> getConnection();

    void sendCommands(const std::vector<CommandMessage>& commands, SafePointer <juce::Component> callbackOwner = nullptr, std::function<void(const std::vector<CommandMessage>& responses)> callback = nullptr);

    const juce::Colour& getTag() const;

    std::shared_ptr<Window> getOrCreateWindow(const juce::ValueTree& windowTree);

    void cleanupWindows();

    juce::String getTitle() const;

    ConnectionPanelContainer& getConnectionPanelContainer();

private:
    const juce::ValueTree& windowLayout;
    std::shared_ptr<ximu3::Connection> connection;
    GLRenderer& glRenderer;
    juce::ThreadPool& threadPool;
    ConnectionPanelContainer& connectionPanelContainer;
    const juce::Colour tag;

    ConnectionPanelHeader header { *this, threadPool, connectionPanelContainer };
    ConnectionPanelFooter footer { *this };

    std::map<juce::Identifier, std::shared_ptr<Window>> windows;
    std::unique_ptr<WindowContainer> windowContainer;

    std::shared_ptr<std::atomic<bool>> destroyed = std::make_shared<std::atomic<bool>>(false);

    void connect();

    void handleAsyncUpdate() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectionPanel)
};
