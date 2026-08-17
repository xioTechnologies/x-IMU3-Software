#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct MarginItem : juce::TreeViewItem {
    bool mightContainSubItems() override { return false; }
    int getItemHeight() const override { return SettingItemComponent::rowMargin / 2; }
};
