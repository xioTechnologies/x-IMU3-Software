#pragma once

#include "../Firmware/Firmware.h"
#include "Dialogs/DataLoggerSettingsDialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <optional>
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"
#include "Widgets/Stopwatch.h"
#include "Ximu3.hpp"

class DevicePanelContainer;

class MenuStrip : public juce::Component,
                  private juce::Timer
{
public:
    MenuStrip(juce::ValueTree& windowLayout_, DevicePanelContainer& devicePanelContainer_);

    void paint(juce::Graphics& g) override;

    void resized() override;

    int getMinimumWidth() const;

private:
    juce::ValueTree& windowLayout;
    DevicePanelContainer& devicePanelContainer;

    IconButton searchButton { BinaryData::search_svg, "Search for Connections", nullptr, false, "" };
    IconButton manualButton { BinaryData::manual_svg, "Manual Connection", std::bind(&MenuStrip::getManualConnectMenu, this) };
    IconButton disconnectButton { BinaryData::disconnect_svg, "Disconnect", std::bind(&MenuStrip::getDisconnectMenu, this) };
    IconButton windowsButton { BinaryData::window_svg, "Windows", std::bind(&MenuStrip::getWindowMenu, this) };
    IconButton windowLayoutButton { BinaryData::layout_svg, "Window Layout", std::bind(&MenuStrip::getWindowLayoutMenu, this) };
    IconButton devicePanelLayoutButton { BinaryData::single_svg, "Device Panel Layout", std::bind(&MenuStrip::getPanelLayoutMenu, this) };
    IconButton shutdownButton { BinaryData::shutdown_svg, "Shutdown All Devices" };
    IconButton sendCommandButton { BinaryData::json_svg, "Send Command", std::bind(&MenuStrip::getSendCommandMenu, this) };
    IconButton dataLoggerStartStopButton { BinaryData::record_svg, "Start Data Logger", nullptr, false, BinaryData::stop_svg, "Stop Data Logger" };
    Stopwatch dataLoggerTime;
    IconButton toolsButton { BinaryData::tools_svg, "Tools", std::bind(&MenuStrip::getToolsMenu, this) };
    IconButton mainSettingsButton { BinaryData::settings_svg, "Application Settings" };
    juce::TextButton versionButton { "v" + juce::JUCEApplication::getInstance()->getApplicationVersion().upToLastOccurrenceOf(".", false, false), "About" };

    SimpleLabel connectionLabel { "Connection", UIFonts::getDefaultFont(), juce::Justification::centred };
    SimpleLabel layoutLabel { "Layout", UIFonts::getDefaultFont(), juce::Justification::centred };
    SimpleLabel commandsLabel { "Commands", UIFonts::getDefaultFont(), juce::Justification::centred };
    SimpleLabel dataLoggerLabel { "Data Logger", UIFonts::getDefaultFont(), juce::Justification::centred };
    SimpleLabel toolsLabel { "Tools", UIFonts::getDefaultFont(), juce::Justification::centred };
    SimpleLabel applicationLabel { "Application", UIFonts::getDefaultFont(), juce::Justification::centred };

    struct ButtonGroup
    {
        SimpleLabel& label;
        const std::vector<juce::Button*> buttons;
    };

    const std::vector<ButtonGroup> buttonGroups {
            { connectionLabel,  { &searchButton,              &manualButton,       &disconnectButton }},
            { layoutLabel,      { &windowsButton,             &windowLayoutButton, &devicePanelLayoutButton }},
            { commandsLabel,    { &shutdownButton,            &sendCommandButton }},
            { dataLoggerLabel,  { &dataLoggerStartStopButton, &dataLoggerTime }},
            { toolsLabel,       { &toolsButton }},
            { applicationLabel, { &mainSettingsButton,        &versionButton }}
    };

    const std::map<DevicePanelContainer::Layout, juce::String> layoutIcons {
            { DevicePanelContainer::Layout::single,    BinaryData::single_svg },
            { DevicePanelContainer::Layout::rows,      BinaryData::rows_svg },
            { DevicePanelContainer::Layout::columns,   BinaryData::columns_svg },
            { DevicePanelContainer::Layout::grid,      BinaryData::grid_svg },
            { DevicePanelContainer::Layout::accordion, BinaryData::accordion_svg },
    };

    DataLoggerSettingsDialog::Settings dataLoggerSettings;
    juce::String dataLoggerName;
    std::unique_ptr<ximu3::DataLogger> dataLogger;
    juce::Time dataLoggerStartTime;

    std::optional<DevicePanelContainer::Layout> preferredMultipleDevicePanelLayout;

    juce::String latestVersion;

    void addDevices(juce::PopupMenu& menu, std::function<void(DevicePanel&)> action);

    void disconnect(const DevicePanel* const);

    juce::PopupMenu getManualConnectMenu();

    juce::PopupMenu getDisconnectMenu();

    juce::PopupMenu getWindowMenu() const;

    juce::PopupMenu getWindowLayoutMenu();

    juce::PopupMenu getPanelLayoutMenu();

    juce::PopupMenu getSendCommandMenu();

    juce::PopupMenu getToolsMenu();

    void setWindowLayout(juce::ValueTree windowLayout_);

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuStrip)
};
