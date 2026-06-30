#pragma once

#include "Widgets/CustomComboBox.h"
#include "SettingValue.h"

class BooleanValue : public SettingValue {
public:
    BooleanValue(DeviceSettingX &setting_) : SettingValue(setting_) {
        addAndMakeVisible(combo);
        combo.addItemList({"false", "true"}, 1);
    }

    void resized() override {
        combo.setBounds(getLocalBounds());
    }

    void refresh() override {
        combo.setSelectedItemIndex(setting.value == "true" ? 1 : 0);
    }

    std::optional<std::string> getValue() const override {
        if (combo.getSelectedItemIndex() == -1) {
            return {};
        }

        return combo.getSelectedItemIndex() == 1 ? "true" : "false";
    }

private:
    CustomComboBox combo;
};
