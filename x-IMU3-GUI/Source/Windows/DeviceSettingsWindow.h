#pragma once

#include "ApplicationSettings.h"
#include "DeviceSettings/DeviceSettings.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/IconButton.h"
#include "Window.h"
#include "Ximu3.hpp"

class DeviceSettingsWindow : public Window
{
public:
    DeviceSettingsWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, ConnectionPanel& connectionPanel_);

    ~DeviceSettingsWindow() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    DeviceSettings deviceSettings;

    IconButton readAllButton { BinaryData::download_svg, "Read Settings from Device", nullptr, false, BinaryData::download_warning_svg, "Read Settings from Device (Failed)" };
    IconButton writeAllButton { BinaryData::upload_svg, "Write Settings to Device", nullptr, false, BinaryData::upload_warning_svg, "Write Settings to Device (Failed)" };
    IconButton saveToFileButton { BinaryData::save_svg, "Save Settings to File" };
    IconButton loadFromFileButton { BinaryData::open_svg, "Load Settings from File" };
    IconButton defaultsButton { BinaryData::default_svg, "Restore Defaults on Device" };

    const std::vector<juce::Component*> buttons { &readAllButton, &writeAllButton, &saveToFileButton, &loadFromFileButton, &defaultsButton };

    juce::Rectangle<float> buttonBounds;

    const juce::File directory = ApplicationSettings::getDirectory().getChildFile("Device Settings");

    void enableInProgress(const std::vector<CommandMessage>& commands);

    void disableInProgress();

    juce::PopupMenu getMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsWindow)
};
