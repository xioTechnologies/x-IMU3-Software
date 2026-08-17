#pragma once

#include "../Window.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Schema/Group.h"
#include "Schema/SettingX.h"
#include "TreeView/GroupItem.h"

class DeviceSettingsXWindow : public Window {
public:
    DeviceSettingsXWindow(const juce::ValueTree &windowLayout, const juce::Identifier &type, ConnectionPanel &connectionPanel_);

    void paint(juce::Graphics &g) override;

    void resized() override;

private:
    struct Tree {
        Tree(Group rootGroup_) : rootGroup(std::move(rootGroup_)) {
            treeView.setRootItem(&rootGroupItem);
            treeView.setRootItemVisible(false);
        }

        Group rootGroup;
        const std::vector<SettingX*> settings = rootGroup.flatten();
        GroupItem rootGroupItem { rootGroup };
        juce::TreeView treeView;
    };

    std::unique_ptr<Tree> tree;

    IconButton syncButton{BinaryData::download_svg, "Sync Settings from Device", nullptr, false, BinaryData::download_warning_svg, "Sync Settings from Device (Failed)"};

    void setGroup(Group group);

    juce::PopupMenu getMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsXWindow)
};
