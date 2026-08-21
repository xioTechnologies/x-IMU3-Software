#pragma once

#include "../Window.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Schema.h"
#include "TreeView/TreeView.h"
#include "Widgets/DisabledOverlay.h"

class DeviceSettingsXWindow : public Window {
public:
    DeviceSettingsXWindow(const juce::ValueTree &windowLayout, const juce::Identifier &type, ConnectionPanel &connectionPanel_);

    void paint(juce::Graphics &g) override;

    void resized() override;

private:
    std::unique_ptr<TreeView> treeView;
    IconButton syncButton{BinaryData::download_svg, "Sync Settings from Device", nullptr, false, BinaryData::download_warning_svg, "Sync Settings from Device (Failed)"};
    DisabledOverlay disabledOverlay;

    struct Settings {
        bool hideUnusedSettings = true;
        bool syncSettingsWhenWindowOpens = true;
    };

    void writeToValueTree(const Settings &settings);

    Settings readFromValueTree();

    void load(std::unique_ptr<Schema::Group> group);

    juce::PopupMenu getMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsXWindow)
};
