#pragma once

#include "SettingItem.h"
#include "GroupItem.h"
#include "MarginItem.h"

class GroupItem : public juce::TreeViewItem {
public:
    GroupItem(Group &g) : group(g) {
        for (auto &item: group.items) {
            if (auto *const group_ = std::get_if<Group>(&item)) {
                addSubItem(new GroupItem(*group_));
            }

            if (auto *const setting = std::get_if<SettingX>(&item)) {
                addSubItem(new SettingItem(*setting));
            }

            if (std::get_if<Margin>(&item)) {
                addSubItem(new MarginItem());
            }
        }
    }

    bool mightContainSubItems() override { return true; }

    std::unique_ptr<juce::Component> createItemComponent() override {
        return std::make_unique<SimpleLabel>("GroupItem " + group.name);
    }

    int getItemHeight() const override {
        return UILayout::textComponentHeight + SettingItemComponent::rowMargin;
    }

private:
    Group &group;
};
