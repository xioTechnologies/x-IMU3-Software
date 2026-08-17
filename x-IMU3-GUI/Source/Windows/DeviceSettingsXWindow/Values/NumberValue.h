#pragma once

#include "Widgets/CustomTextEditor.h"
#include "ValueBase.h"

class NumberValue : public ValueBase {
public:
    NumberValue(SettingX &setting_) : ValueBase(setting_) {
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
