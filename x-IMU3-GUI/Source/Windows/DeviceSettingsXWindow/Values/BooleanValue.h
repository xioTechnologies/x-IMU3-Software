#pragma once

#include "Widgets/CustomComboBox.h"
#include "SettingValue.h"

class BooleanValue : public SettingValue {
public:
    BooleanValue(SettingX &setting_) : SettingValue(setting_) {
        addAndMakeVisible(combo);
        combo.addItemList({"false", "true"}, 1);
        combo.onChange = [&] {
            if (combo.getSelectedItemIndex() == -1) {
                setting.uiValue = {};
                return;
            }

            setting.uiValue = combo.getSelectedItemIndex() == 1 ? "true" : "false";
        };
    }

    void resized() override {
        combo.setBounds(getLocalBounds());
    }

    void refresh() override {
        combo.setSelectedItemIndex(setting.value == "true" ? 1 : 0);
    }

private:
    CustomComboBox combo;
};
