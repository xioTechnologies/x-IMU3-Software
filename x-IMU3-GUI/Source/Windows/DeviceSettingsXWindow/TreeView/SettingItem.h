#pragma once

#include "TreeViewItem.h"

class SettingItem : public TreeViewItem {
public:
    SettingItem(Schema::Setting &setting_, Schema::Setting *const dependsOn_, std::function<void(Schema::Setting &setting, const std::string &command)> write_) : TreeViewItem(dependsOn_, setting_.dependsOnValues),
                                                                                                                                                                setting(setting_),
                                                                                                                                                                write(std::move(write_)) {
    }

    bool mightContainSubItems() override {
        return false;
    }

    std::unique_ptr<juce::Component> createItemComponent() override {
        return std::make_unique<SettingItemComponent>(setting, write);
    }

private:
    Schema::Setting &setting;
    const std::function<void(Schema::Setting &setting, const std::string &command)> write;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingItem)
};
