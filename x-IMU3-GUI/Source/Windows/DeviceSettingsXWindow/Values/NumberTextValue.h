#pragma once

#include "Widgets/CustomTextEditor.h"
#include "SettingValue.h"

class NumberTextValue : public SettingValue {
public:
    NumberTextValue(DeviceSettingX &setting_) : SettingValue(setting_) {
        addAndMakeVisible(editor);
    }

    void resized() override {
        editor.setBounds(getLocalBounds());
    }

    void refresh() override {
        editor.setText(setting.value);
    }

    std::optional<std::string> getValue() const override {
        return editor.getText().toStdString();
    }

private:
    CustomTextEditor editor;
};
