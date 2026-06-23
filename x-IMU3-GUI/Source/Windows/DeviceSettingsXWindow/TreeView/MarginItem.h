#pragma once

class MarginItem : public juce::TreeViewItem {
public:
    bool mightContainSubItems() override { return false; }
    int getItemHeight() const override { return SettingComponent::rowMargin / 2; }
};

