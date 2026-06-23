#pragma once

#include "Widgets/CustomComboBox.h"
#include "SettingValue.h"

class EnumValue : public SettingValue {
public:
    EnumValue(SettingX &setting_) : SettingValue(setting_) {
        addAndMakeVisible(combo);
        for (auto it: setting.numberEnum) {
            combo.addItem(it.second, 1 + combo.getNumItems());
        }
        combo.onChange = [&] {
            if (combo.getSelectedItemIndex() == -1) {
                setting.uiValue = {};
                return;
            }

            setting.uiValue = std::to_string(setting.numberEnum[(size_t) combo.getSelectedItemIndex()].first);
        };
    }

    void resized() override {
        combo.setBounds(getLocalBounds());
    }

    void refresh() override {
        for (size_t index = 0; index < setting.numberEnum.size(); index++) {
            if (std::to_string(setting.numberEnum[index].first) == setting.value) {
                combo.setSelectedItemIndex((int) index);
            }
        }
    }

private:
    CustomComboBox combo;
};
