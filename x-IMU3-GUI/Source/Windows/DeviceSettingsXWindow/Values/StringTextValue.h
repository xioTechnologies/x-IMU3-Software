#pragma once

#include "Widgets/CustomTextEditor.h"
#include "SettingValue.h"

class StringTextValue : public SettingValue {
public:
    StringTextValue(DeviceSettingX &setting_) : SettingValue(setting_) {
        addAndMakeVisible(editor);
    }

    void resized() override {
        editor.setBounds(getLocalBounds());
    }

    void refresh() override {
        if (setting.value.size() >= 2 && setting.value.front() == '"' && setting.value.back() == '"') {
            editor.setText(setting.value.substr(1, setting.value.size() - 2));
            return;
        }

        // TODO: handle else
    }

    std::optional<std::string> getValue() const override {
        return "\"" + editor.getText().toStdString() + "\"";
    }

private:
    CustomTextEditor editor;
};
