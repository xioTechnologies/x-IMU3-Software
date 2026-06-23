#pragma once

#include "SettingItemComponent.h"
#include "TreeViewItem.h"

class SettingItem : public TreeViewItem {
public:
    SettingItem(Schema::Setting &setting_, Schema::Setting *const dependsOn_, std::function<void(Schema::Setting & setting, const std::string & command)> write_, std::function<bool()> hideUnusedSettings_)
        : TreeViewItem(setting_, dependsOn_, setting_.dependsOnValues, hideUnusedSettings_),
          setting(setting_),
          write(std::move(write_)) {
    }

    bool mightContainSubItems() override {
        return false;
    }

    std::unique_ptr<juce::Component> createItemComponent() override {
        auto component_ = std::make_unique<SettingItemComponent>(setting, write);
        component = component_.get();
        return component_;
    }

    void refresh() override {
        TreeViewItem::refresh();

        if (component) {
            component->refresh();
        }
    }

private:
    Schema::Setting &setting;
    const std::function<void(Schema::Setting & setting, const std::string & command)> write;
    juce::Component::SafePointer<SettingItemComponent> component;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingItem)
};
