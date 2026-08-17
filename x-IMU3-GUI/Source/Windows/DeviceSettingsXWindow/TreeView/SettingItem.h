#pragma once

#include "SettingItemComponent.h"

class SettingItem : public juce::TreeViewItem
{
public:
    SettingItem(SettingX& setting_) : setting(setting_)
    {
    }

    bool mightContainSubItems() override { return false; }

    std::unique_ptr<juce::Component> createItemComponent() override {
        return std::make_unique<SettingItemComponent>(setting);;
    }

    int getItemHeight() const override
    {
        return UILayout::textComponentHeight + SettingItemComponent::rowMargin;
    }

private:
    SettingX& setting;
};
