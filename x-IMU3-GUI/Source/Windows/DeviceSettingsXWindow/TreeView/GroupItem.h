#pragma once

#include "SettingItem.h"
#include "GroupItem.h"
#include "MarginItem.h"
#include "SettingComponent.h"

class GroupItem : public juce::TreeViewItem
{
public:
    GroupItem(Group& g) : group(g)
    {
        for (auto &item: group.items) {
            if (auto* const group_ = std::get_if<Group>(&item)) {
                addSubItem(new GroupItem(*group_));
            }

            if (auto* const setting = std::get_if<SettingX>(&item)) {
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

    int getItemHeight() const override
    {
        return UILayout::textComponentHeight + SettingComponent::rowMargin;
    }

    std::vector<SettingItem*> flatten(juce::TreeView& treeView) {
        std::vector<SettingItem*> vector;

        for (int index = 0; index < getNumSubItems(); index++) {
            if (auto* item = dynamic_cast<GroupItem*>(getSubItem(index))) {
                const auto flattened = item->flatten(treeView);
                vector.insert(vector.end(), flattened.begin(), flattened.end());
            }

            if (auto* item = dynamic_cast<SettingItem*>(getSubItem(index))) {
                vector.push_back(item);
            }
        }

        return vector;
    }

    Group& group;
};
