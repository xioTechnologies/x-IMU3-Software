#pragma once

#include <algorithm>
#include "../Schema.h"
#include "SettingItemComponent.h"

class TreeViewItem : public juce::TreeViewItem {
public:
    TreeViewItem(const Schema::Item &item_, Schema::Setting *const dependsOn_, std::vector<std::string> dependsOnValues_, std::function<bool()> hideUnusedSettings_)
        : item(item_),
          dependsOn(dependsOn_),
          dependsOnValues(std::move(dependsOnValues_)),
          hideUnusedSettings(hideUnusedSettings_) {
    }

    int getItemHeight() const override {
        if (const auto *const parent = getParentItem(); parent != nullptr && parent->getItemHeight() == 0) {
            return 0;
        }

        if (unused && hideUnusedSettings()) {
            return 0;
        }

        return UILayout::textComponentHeight + SettingItemComponent::rowMargin;
    }

    bool canBeSelected() const override {
        return false; // fix return key expanding/collapsing groups
    }

    virtual void refresh() {
        if (item.getError().has_value()) {
            unused = false;
            return;
        }

        if (dependsOn != nullptr && dependsOn->status == Schema::Setting::Status::confirmed) {
            unused = std::find(dependsOnValues.begin(), dependsOnValues.end(), dependsOn->value) == dependsOnValues.end();
        }
    }

private:
    const Schema::Item &item;
    Schema::Setting *const dependsOn;
    const std::vector<std::string> dependsOnValues;
    std::function<bool()> hideUnusedSettings;

    bool unused = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeViewItem)
};
