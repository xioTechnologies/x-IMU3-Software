#pragma once

#include <algorithm>
#include "SettingItemComponent.h"

class SettingItem : public juce::TreeViewItem,
                    private juce::ChangeListener {
public:
    SettingItem(Schema::Setting &setting_, Schema::Setting *const dependsOn_, std::function<void(Schema::Setting &setting, const std::string &command)> write_) : setting(setting_),
                                                                                                                                                                  dependsOn(dependsOn_),
                                                                                                                                                                  write(std::move(write_)) {
        if (dependsOn != nullptr) {
            dependsOn->addChangeListener(this);
        }
    }

    ~SettingItem() override {
        if (dependsOn != nullptr) {
            dependsOn->removeChangeListener(this);
        }
    }

    bool mightContainSubItems() override {
        return false;
    }

    std::unique_ptr<juce::Component> createItemComponent() override {
        return std::make_unique<SettingItemComponent>(setting, write);
    }

    int getItemHeight() const override {
        if (isHidden()) {
            return 0;
        }

        return UILayout::textComponentHeight + SettingItemComponent::rowMargin;
    }

private:
    Schema::Setting &setting;
    Schema::Setting *const dependsOn;
    const std::function<void(Schema::Setting &setting, const std::string &command)> write;

    bool isHidden() const {
        if (const auto *const parent = getParentItem(); parent != nullptr && parent->getItemHeight() == 0) {
            return true;
        }

        if (dependsOn == nullptr || dependsOn->status != Schema::Setting::Status::confirmed) {
            return false;
        }

        return std::find(setting.dependsOnValues.begin(), setting.dependsOnValues.end(), dependsOn->value) == setting.dependsOnValues.end();
    }

    void changeListenerCallback(juce::ChangeBroadcaster *) override {
        treeHasChanged();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingItem)
};
