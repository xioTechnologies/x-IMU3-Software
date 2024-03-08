#pragma once

#include "Dialogs/DataLoggerSettingsDialog.h"
#include "Firmware/Firmware.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <optional>
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"
#include "Widgets/Stopwatch.h"
#include "Ximu3.hpp"

class ConnectionPanelContainer;

class MenuStrip : public juce::Component,
                  private juce::Timer
{
public:
    MenuStrip(juce::ValueTree& windowLayout_, juce::ThreadPool& threadPool_, ConnectionPanelContainer& connectionPanelContainer_);

    ~MenuStrip() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

    int getMinimumWidth() const;

private:
    juce::ValueTree& windowLayout;
    juce::ThreadPool& threadPool;
    ConnectionPanelContainer& connectionPanelContainer;

    IconButton availableConnectionsButton { BinaryData::search_svg, "Available Connections" };
    IconButton manualConnectionButton { BinaryData::manual_svg, "Manual Connection", std::bind(&MenuStrip::getManualConnectionMenu, this) };
    IconButton disconnectButton { BinaryData::disconnect_svg, "Disconnect", std::bind(&MenuStrip::getDisconnectMenu, this) };
    IconButton connectionLayoutButton { BinaryData::grid_svg, "Connection Layout", std::bind(&MenuStrip::getConnectionLayoutMenu, this) };
    IconButton windowsButton { BinaryData::window_svg, "Windows", std::bind(&MenuStrip::getWindowMenu, this) };
    IconButton shutdownButton { BinaryData::shutdown_svg, "Send Shutdown Command to All" };
    IconButton zeroHeadingButton { BinaryData::north_svg, "Send Zero Heading Command to All" };
    IconButton noteButton { BinaryData::note_svg, "Send Note Command to All" };
    IconButton sendCommandButton { BinaryData::json_svg, "Send Command", std::bind(&MenuStrip::getSendCommandMenu, this) };
    IconButton dataLoggerStartStopButton { BinaryData::record_svg, "Start Data Logger", nullptr, false, BinaryData::stop_svg, "Stop Data Logger" };
    Stopwatch dataLoggerTime;
    IconButton toolsButton { BinaryData::tools_svg, "Tools", std::bind(&MenuStrip::getToolsMenu, this) };
    IconButton mainSettingsButton { BinaryData::settings_svg, "Application Settings" };
    IconButton aboutButton { BinaryData::xio_icon_svg, "About", nullptr, false, BinaryData::xio_icon_update_svg, "About (Update Available)" };

    const std::vector<std::vector<juce::Button*>> buttonGroups {
            { &availableConnectionsButton, &manualConnectionButton, &disconnectButton, &connectionLayoutButton },
            { &windowsButton },
            { &shutdownButton,             &zeroHeadingButton,      &noteButton,       &sendCommandButton },
            { &dataLoggerStartStopButton,  &dataLoggerTime },
            { &toolsButton },
            { &mainSettingsButton,         &aboutButton },
    };

    const std::map<ConnectionPanelContainer::Layout, juce::String> layoutIcons {
            { ConnectionPanelContainer::Layout::rows,      BinaryData::rows_svg },
            { ConnectionPanelContainer::Layout::columns,   BinaryData::columns_svg },
            { ConnectionPanelContainer::Layout::grid,      BinaryData::grid_svg },
            { ConnectionPanelContainer::Layout::accordion, BinaryData::accordion_svg },
    };

    DataLoggerSettingsDialog::Settings dataLoggerSettings;
    juce::String dataLoggerName;
    std::unique_ptr<ximu3::DataLogger> dataLogger;
    juce::Time dataLoggerStartTime;

    juce::String latestVersion;

    juce::File previousWindowLayout = ApplicationSettings::getDirectory().getChildFile("Previous Windows Layout.xml");

    void addDevices(juce::PopupMenu& menu, std::function<void(ConnectionPanel&)> action);

    void disconnect(const ConnectionPanel* const);

    juce::PopupMenu getManualConnectionMenu();

    juce::PopupMenu getDisconnectMenu();

    juce::PopupMenu getConnectionLayoutMenu();

    juce::PopupMenu getWindowMenu();

    juce::PopupMenu getSendCommandMenu();

    juce::PopupMenu getToolsMenu();

    void setWindowLayout(juce::ValueTree windowLayout_);

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuStrip)
};
