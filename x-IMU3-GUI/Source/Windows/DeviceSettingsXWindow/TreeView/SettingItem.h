#pragma once

#include "SettingComponent.h"

class SettingItem : public juce::TreeViewItem
{
public:
    SettingItem(SettingX& setting_) : setting(setting_)
    {
    }

    bool mightContainSubItems() override { return false; }

    std::unique_ptr<juce::Component> createItemComponent() override {
        auto result = std::make_unique<SettingComponent>(setting);;
        component = result.get();
        return result;
    }

    int getItemHeight() const override
    {
        return UILayout::textComponentHeight + SettingComponent::rowMargin;
    }

    void refresh() {
        if (component) {
            component->refresh();
        }
    }

private:
    SettingX& setting;
    juce::Component::SafePointer<SettingComponent> component;
};
