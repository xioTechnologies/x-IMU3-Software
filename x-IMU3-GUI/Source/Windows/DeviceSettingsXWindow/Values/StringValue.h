#pragma once

#include "Widgets/CustomTextEditor.h"
#include "ValueBase.h"

class StringValue : public ValueBase {
public:
    StringValue(SettingX &setting_) : ValueBase(setting_) {
        addAndMakeVisible(editor);
        editor.onReturnKey = [&] {
            setting.onWrite(setting.writeCommand(('"' + editor.getText() + '"').toStdString()));
            editor.setText("", false);
        };
    }

    void resized() override {
        editor.setBounds(getLocalBounds());
    }

    void refresh() override {
        if (setting.value.size() >= 2 && setting.value.front() == '"' && setting.value.back() == '"') {
            editor.setText(setting.value.substr(1, setting.value.size() - 2), false);
            return;
        }
    }

private:
    CustomTextEditor editor;
};
