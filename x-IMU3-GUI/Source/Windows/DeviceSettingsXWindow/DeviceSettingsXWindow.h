#pragma once

#include "../Window.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Schema/Group.h"
#include "Schema/SettingX.h"
#include "TreeView/GroupItem.h"
#include "TreeView/SettingComponent.h"

class DeviceSettingsXWindow : public Window {
public:
    DeviceSettingsXWindow(const juce::ValueTree &windowLayout, const juce::Identifier &type, ConnectionPanel &connectionPanel_);

    void paint(juce::Graphics &g) override;

    void resized() override;

private:
    struct Tree {
        Tree(Group rootGroup_) : rootGroup(rootGroup_) {
            treeView.setRootItem(&rootGroupItem);
            settingItems = rootGroupItem.flatten(treeView);
        }

        Group rootGroup;
        GroupItem rootGroupItem { rootGroup };
        std::vector<SettingX*> settings = rootGroup.flatten();
        std::vector<SettingItem*> settingItems;
        juce::TreeView treeView;
    };

    std::unique_ptr<Tree> tree;

    IconButton readAllButton{BinaryData::download_svg, "Read Settings from Device", nullptr, false, BinaryData::download_warning_svg, "Read Settings from Device (Failed)"};
    IconButton writeAllButton{BinaryData::upload_svg, "Write Settings to Device", nullptr, false, BinaryData::upload_warning_svg, "Write Settings to Device (Failed)"};

    void setGroup(Group group);

    juce::PopupMenu getMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsXWindow)
};
