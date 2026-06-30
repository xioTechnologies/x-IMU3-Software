#pragma once

#include "Widgets/CustomComboBox.h"
#include "SettingValue.h"

class EnumValue : public SettingValue {
public:
    EnumValue(DeviceSettingX &setting_) : SettingValue(setting_) {
        addAndMakeVisible(combo);
        for (auto it: setting.numberEnum) {
            combo.addItem(it.second, 1 + combo.getNumItems());
        }
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

    std::optional<std::string> getValue() const override {
        if (combo.getSelectedItemIndex() == -1) {
            return {};
        }

        return std::to_string(setting.numberEnum[(size_t) combo.getSelectedItemIndex()].first);
    }

private:
    CustomComboBox combo;
};
