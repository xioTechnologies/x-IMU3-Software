#pragma once

#include "../SettingsSchema.h"
#include "GroupItem.h"
#include <juce_gui_basics/juce_gui_basics.h>

class TreeView : public juce::TreeView {
public:
    TreeView(std::unique_ptr<SettingsSchema::Group> rootGroup_, const std::function<void(SettingsSchema::Setting &setting, const std::string &command)> &write, std::function<bool()> hideUnusedSettings)
        : rootGroup(std::move(rootGroup_)),
          rootGroupItem(*rootGroup, *rootGroup, write, hideUnusedSettings) {
        setRootItem(&rootGroupItem);
        setRootItemVisible(false);
    }

    ~TreeView() override {
        setRootItem(nullptr);
    }

    const std::vector<SettingsSchema::Setting *> &getSettings() const {
        return settings;
    }

    void refresh() {
        rootGroupItem.refresh();
        rootGroupItem.treeHasChanged();
    }

    void expandOrCollapseAll(const bool expand) {
        rootGroupItem.expandOrCollapseAll(expand);
    }

private:
    std::unique_ptr<SettingsSchema::Group> rootGroup;
    const std::vector<SettingsSchema::Setting *> settings = rootGroup->flatten();
    GroupItem rootGroupItem;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeView)
};
