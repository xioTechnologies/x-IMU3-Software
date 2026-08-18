#pragma once

#include "GroupItem.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Schema.h"

class TreeView : public juce::TreeView {
public:
    TreeView(Schema::Group rootGroup_, const std::function<void(Schema::Setting &setting, const std::string &command)> &write) : rootGroup(std::move(rootGroup_)),
                                                                                                                                rootGroupItem(rootGroup, rootGroup, write) {
        setRootItem(&rootGroupItem);
        setRootItemVisible(false);
    }

    ~TreeView() override {
        setRootItem(nullptr);
    }

    const std::vector<Schema::Setting *> &getSettings() const {
        return settings;
    }

private:
    Schema::Group rootGroup;
    const std::vector<Schema::Setting *> settings = rootGroup.flatten();
    GroupItem rootGroupItem;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeView)
};
