#pragma once

#include "Widgets/CustomTextEditor.h"
#include "SettingValue.h"

class StringTextValue : public SettingValue {
public:
    StringTextValue(SettingX &setting_) : SettingValue(setting_) {
        addAndMakeVisible(editor);
        editor.onTextChange = [&] {
            setting.uiValue = "\"" + editor.getText().toStdString() + "\"";
        };
    }

    void resized() override {
        editor.setBounds(getLocalBounds());
    }

    void refresh() override {
        if (setting.uiValue.size() >= 2 && setting.uiValue.front() == '"' && setting.uiValue.back() == '"') {
            editor.setText(setting.uiValue.substr(1, setting.uiValue.size() - 2));
            return;
        }

        // TODO: handle else
    }

private:
    CustomTextEditor editor;
};
