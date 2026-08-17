#pragma once

#include "Widgets/CustomTextEditor.h"
#include "Value.h"

class NumberValue : public Value {
public:
    NumberValue(SettingX &setting_) : Value(setting_) {
        addAndMakeVisible(editor);
        editor.onTextChange = [&] {
            // TODO: Send and go to unknown state
        };
    }

    void resized() override {
        editor.setBounds(getLocalBounds());
    }

    void refresh() override {
        editor.setText(setting.value, false);
    }

private:
    CustomTextEditor editor;
};
