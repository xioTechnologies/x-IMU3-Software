#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Schema.h"
#include "Value.h"
#include "Widgets/CustomTextEditor.h"

class NumberValue final : public CustomTextEditor,
                          public Value {
public:
    NumberValue(Schema::Setting &setting_, std::function<void(Schema::Setting &setting, const std::string &command)> write) : setting(setting_) {
        setReadOnly(setting.readOnly);
        setDefaultText(setting.emptyString);
        onReturnKey = onEscapeKey = onFocusLost = [&, write] {
            if (setting.readOnly) {
                return;
            }

            setting.clear();
            write(setting, setting.getWrite(getText().toStdString()));
        };

        refresh();
    }

    void refresh() override {
        if (setting.status != Schema::Setting::Status::confirmed) {
            setText({}, false);
            return;
        }

        setText(setting.value, false);
    }

private:
    Schema::Setting &setting;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NumberValue)
};
