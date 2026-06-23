#pragma once

#include "../SettingsSchema.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Value.h"
#include "Widgets/CustomTextEditor.h"

class StringValue final : public CustomTextEditor,
                          public Value {
public:
    StringValue(SettingsSchema::Setting &setting_, std::function<void(SettingsSchema::Setting &setting, const std::string &command)> write) : setting(setting_) {
        setReadOnly(setting.readOnly);
        setDefaultText(setting.emptyString);
        onReturnKey = onEscapeKey = [&] {
            giveAwayKeyboardFocus();
        };
        onFocusLost = [&, write] {
            const auto value = "\"" + getText().toStdString() + "\"";

            if (setting.value == value && setting.status == SettingsSchema::Setting::Status::confirmed) {
                return;
            }

            setting.clear();
            write(setting, setting.getWriteCommand(value));
        };

        refresh();
    }

    void refresh() override {
        if (setting.status != SettingsSchema::Setting::Status::confirmed) {
            setText({}, false);
            return;
        }

        setText(setting.getStringValue(), false);
    }

private:
    SettingsSchema::Setting &setting;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringValue)
};
