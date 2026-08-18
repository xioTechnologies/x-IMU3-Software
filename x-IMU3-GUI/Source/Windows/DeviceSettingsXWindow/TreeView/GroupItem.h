#pragma once

#include "../Schema.h"
#include "SettingItem.h"

class GroupItem : public TreeViewItem {
public:
    GroupItem(const Schema::Group &group_, const Schema::Group &rootGroup, const std::function<void(Schema::Setting &setting, const std::string &command)> &write) : TreeViewItem(rootGroup.find(group_.dependsOnKey), group_.dependsOnValues),
                                                                                                                                                                     group(group_) {
        setLinesDrawnForSubItems(false);

        for (const auto &item: group.items) {
            if (const auto *const subGroup = std::get_if<Schema::Group>(&item)) {
                addSubItem(new GroupItem(*subGroup, rootGroup, write));
                continue;
            }

            if (const auto *const setting = std::get_if<std::unique_ptr<Schema::Setting> >(&item)) {
                addSubItem(new SettingItem(**setting, rootGroup.find((*setting)->dependsOnKey), write));
            }
        }

        setOpen(group.expand);
    }

    bool mightContainSubItems() override {
        return true;
    }

    std::unique_ptr<juce::Component> createItemComponent() override {
        return std::make_unique<SimpleLabel>(group.name);
    }

private:
    const Schema::Group &group;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GroupItem)
};
