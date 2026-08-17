#pragma once

#include "Widgets/CustomTextEditor.h"
#include "Value.h"

class StringValue : public Value {
public:
    StringValue(SettingX &setting_, std::function<void(std::string)> write) : Value(setting_) {
        addAndMakeVisible(editor);
        editor.onReturnKey = [&, write] {
            setting.clear();

            write(setting.writeCommand(('"' + editor.getText() + '"').toStdString()));
        };
    }

    void resized() override {
        editor.setBounds(getLocalBounds());
    }

    void refresh() override {
        // TODO: Clear when status unknown

        if (setting.value.size() >= 2 && setting.value.front() == '"' && setting.value.back() == '"') {
            editor.setText(setting.value.substr(1, setting.value.size() - 2), false);
            return;
        }
    }

private:
    CustomTextEditor editor;
};
