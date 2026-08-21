#pragma once

#include <algorithm>
#include "../Schema.h"
#include "SettingItemComponent.h"

class TreeViewItem : public juce::TreeViewItem,
                     private juce::ChangeListener {
public:
    TreeViewItem(Schema::Setting *const dependsOn_, std::vector<std::string> dependsOnValues_, std::function<bool()> hideUnusedSettings_)
        : dependsOn(dependsOn_),
          dependsOnValues(std::move(dependsOnValues_)),
          hideUnusedSettings(hideUnusedSettings_) {
        if (dependsOn != nullptr) {
            dependsOn->addChangeListener(this);
        }
    }

    ~TreeViewItem() override {
        if (dependsOn != nullptr) {
            dependsOn->removeChangeListener(this);
        }
    }

    int getItemHeight() const override {
        if (const auto *const parent = getParentItem(); parent != nullptr && parent->getItemHeight() == 0) {
            return 0;
        }

        if (hide && hideUnusedSettings()) {
            return 0;
        }

        return UILayout::textComponentHeight + SettingItemComponent::rowMargin;
    }

private:
    Schema::Setting *const dependsOn;
    const std::vector<std::string> dependsOnValues;
    std::function<bool()> hideUnusedSettings;

    bool hide = false;

    void changeListenerCallback(juce::ChangeBroadcaster *) override {
        if (dependsOn->status != Schema::Setting::Status::confirmed) {
            return;
        }

        hide = std::find(dependsOnValues.begin(), dependsOnValues.end(), dependsOn->value) == dependsOnValues.end();
        treeHasChanged();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TreeViewItem)
};
