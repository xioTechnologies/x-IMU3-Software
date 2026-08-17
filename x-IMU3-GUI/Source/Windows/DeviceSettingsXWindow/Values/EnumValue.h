#pragma once

#include "Widgets/CustomComboBox.h"
#include "ValueBase.h"

class EnumValue : public ValueBase {
public:
    EnumValue(SettingX &setting_) : ValueBase(setting_) {
        addAndMakeVisible(combo);
        for (auto it: setting.numberEnum) {
            combo.addItem(it.second, 1 + combo.getNumItems());
        }
        combo.onChange = [&] {
            // TODO: Send and go to unknown state
        };
    }

    void resized() override {
        combo.setBounds(getLocalBounds());
    }

    void refresh() override {
        for (size_t index = 0; index < setting.numberEnum.size(); index++) {
            if (std::to_string(setting.numberEnum[index].first) == setting.value) {
                combo.setSelectedItemIndex((int) index, juce::dontSendNotification);
            }
        }
    }

private:
    CustomComboBox combo;
};
