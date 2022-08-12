#pragma once

#include "../Firmware/Firmware.h"
#include "Dialogs/DataLoggerSettingsDialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
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

private:
    juce::ValueTree& windowLayout;
    DevicePanelContainer& devicePanelContainer;

    IconButton searchButton { IconButton::Style::menuStrip, BinaryData::search_svg, 1.0f, "Search for Connections", nullptr, "" };
    IconButton manualButton { IconButton::Style::menuStripDropdown, BinaryData::manual_svg, 1.0f, "Manual Connection", std::bind(&MenuStrip::getManualConnectMenu, this) };
    IconButton disconnectButton { IconButton::Style::menuStripDropdown, BinaryData::disconnect_svg, 1.0f, "Disconnect", std::bind(&MenuStrip::getDisconnectMenu, this) };
    IconButton showHideWindowButton { IconButton::Style::menuStripDropdown, BinaryData::window_svg, 1.0f, "Show/Hide Windows", std::bind(&MenuStrip::getWindowMenu, this) };
    IconButton windowLayoutButton { IconButton::Style::menuStripDropdown, BinaryData::layout_svg, 1.0f, "Window Layout", std::bind(&MenuStrip::getWindowLayoutMenu, this) };
    IconButton devicePanelLayoutButton { IconButton::Style::menuStripDropdown, BinaryData::rows_svg, 1.0f, "Device Panel Layout", std::bind(&MenuStrip::getPanelLayoutMenu, this) };
    IconButton shutdownButton { IconButton::Style::menuStrip, BinaryData::shutdown_svg, 0.8f, "Shutdown All Devices" };
    IconButton sendCommandButton { IconButton::Style::menuStrip, BinaryData::json_svg, 0.8f, "Send Command to All Devices" };
    IconButton dataLoggerStartStopButton { IconButton::Style::menuStrip, BinaryData::record_svg, 0.8f, "Start Data Logger", nullptr, BinaryData::stop_svg, 0.8f, "Stop Data Logger" };
    Stopwatch dataLoggerTime;
    IconButton toolsButton { IconButton::Style::menuStripDropdown, BinaryData::tools_svg, 1.0f, "Tools", std::bind(&MenuStrip::getToolsMenu, this) };
    IconButton mainSettingsButton { IconButton::Style::menuStrip, BinaryData::settings_svg, 1.0f, "Application Settings" };
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
        const std::vector<std::reference_wrapper<juce::Component>> buttons;
        juce::FlexBox groupBox, buttonBox;
    };

    std::vector<ButtonGroup> buttonGroups {
            { connectionLabel,  { searchButton,              manualButton,       disconnectButton },        {}, {}},
            { layoutLabel,      { showHideWindowButton,      windowLayoutButton, devicePanelLayoutButton }, {}, {}},
            { commandsLabel,    { shutdownButton,            sendCommandButton },                           {}, {}},
            { dataLoggerLabel,  { dataLoggerStartStopButton, dataLoggerTime },                              {}, {}},
            { toolsLabel,       { toolsButton },                                                            {}, {}},
            { applicationLabel, { mainSettingsButton,        versionButton },                               {}, {}}
    };

    const std::map<DevicePanelContainer::Layout, juce::String> layoutIcons {
            { DevicePanelContainer::Layout::rows,      BinaryData::rows_svg },
            { DevicePanelContainer::Layout::columns,   BinaryData::columns_svg },
            { DevicePanelContainer::Layout::grid,      BinaryData::grid_svg },
            { DevicePanelContainer::Layout::accordion, BinaryData::accordion_svg },
    };

    DataLoggerSettingsDialog::Settings dataLoggerSettings;
    std::unique_ptr<ximu3::DataLogger> dataLogger;
    juce::Time dataLoggerStartTime;

    void disconnect(const DevicePanel* const);

    juce::PopupMenu getManualConnectMenu();

    juce::PopupMenu getDisconnectMenu();

    juce::PopupMenu getWindowMenu() const;

    juce::PopupMenu getWindowLayoutMenu();

    juce::PopupMenu getPanelLayoutMenu();

    juce::PopupMenu getToolsMenu();

    void setWindowLayout(juce::ValueTree windowLayout_);

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MenuStrip)
};
