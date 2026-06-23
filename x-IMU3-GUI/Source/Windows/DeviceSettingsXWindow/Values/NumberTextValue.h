#pragma once

#include "Widgets/CustomTextEditor.h"
#include "SettingValue.h"

class NumberTextValue : public SettingValue {
public:
    NumberTextValue(SettingX &setting_) : SettingValue(setting_) {
        addAndMakeVisible(editor);
        editor.onTextChange = [&] {
            setting.uiValue = editor.getText().toStdString();
        };
    }

    void resized() override {
        editor.setBounds(getLocalBounds());
    }

    void refresh() override {
        editor.setText(setting.value);
    }

private:
    CustomTextEditor editor;
};
