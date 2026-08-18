#pragma once

#include "../Window.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Schema.h"
#include "TreeView/TreeView.h"

class DeviceSettingsXWindow : public Window {
public:
    DeviceSettingsXWindow(const juce::ValueTree &windowLayout, const juce::Identifier &type, ConnectionPanel &connectionPanel_);

    void paint(juce::Graphics &g) override;

    void resized() override;

private:
    std::unique_ptr<TreeView> treeView;
    IconButton syncButton{BinaryData::download_svg, "Sync Settings from Device", nullptr, false, BinaryData::download_warning_svg, "Sync Settings from Device (Failed)"};

    void load(Schema::Group group);

    juce::PopupMenu getMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsXWindow)
};
