#pragma once

#include "../Window.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "DeviceSettingX.h"
#include "SettingComponent.h"

class DeviceSettingsXWindow : public Window {
public:
    DeviceSettingsXWindow(const juce::ValueTree &windowLayout, const juce::Identifier &type, ConnectionPanel &connectionPanel_);

    void paint(juce::Graphics &g) override;

    void resized() override;

private:
    std::vector<DeviceSettingX> settings;
    std::vector<std::unique_ptr<SettingComponent> > components;

    IconButton readAllButton{BinaryData::download_svg, "Read Settings from Device", nullptr, false, BinaryData::download_warning_svg, "Read Settings from Device (Failed)"};
    IconButton writeAllButton{BinaryData::upload_svg, "Write Settings to Device", nullptr, false, BinaryData::upload_warning_svg, "Write Settings to Device (Failed)"};

    void setSettings(const std::vector<DeviceSettingX> &settings_);

    juce::PopupMenu getMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsXWindow)
};
