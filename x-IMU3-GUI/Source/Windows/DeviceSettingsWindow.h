#pragma once

#include "../ApplicationSettings.h"
#include "../Widgets/IconButton.h"
#include "DeviceSettings/DeviceSettings.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Window.h"
#include "Ximu3.hpp"

class DeviceSettingsWindow : public Window
{
public:
    DeviceSettingsWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_);

    ~DeviceSettingsWindow() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    DeviceSettings settingsTree;

    IconButton readAllButton { IconButton::Style::normal, BinaryData::download_svg, 1.0f, "Read Settings From Device" };
    IconButton writeAllButton { IconButton::Style::normal, BinaryData::upload_svg, 1.0f, "Write Settings To Device" };
    IconButton saveToFileButton { IconButton::Style::normal, BinaryData::save_svg, 1.0f, "Save Settings To File" };
    IconButton loadFromFileButton { IconButton::Style::normal, BinaryData::open_svg, 1.0f, "Load Settings From File" };
    IconButton defaultsButton { IconButton::Style::normal, BinaryData::default_svg, 1.0f, "Restore Default Settings" };

    std::function<void(const std::vector<CommandMessage>&, const std::vector<CommandMessage>&)> sendCommandsCallback;

    const juce::File directory = ApplicationSettings::getDirectory().getChildFile("Device Settings");

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsWindow)
};
