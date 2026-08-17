#pragma once

#include "Widgets/CustomComboBox.h"
#include "Value.h"

class BooleanValue : public Value {
public:
    BooleanValue(SettingX &setting_) : Value(setting_) {
        addAndMakeVisible(combo);
        combo.addItemList({"false", "true"}, 1);
        combo.onChange = [&] {
            // TODO: Send and go to unknown state
        };
    }

    void resized() override {
        combo.setBounds(getLocalBounds());
    }

    void refresh() override {
        combo.setSelectedItemIndex(setting.value == "true" ? 1 : 0, juce::dontSendNotification);
    }

private:
    CustomComboBox combo;
};
