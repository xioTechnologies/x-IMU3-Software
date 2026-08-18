#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Schema.h"
#include "Widgets/CustomTextEditor.h"

class StringValue : public CustomTextEditor,
                    private juce::ChangeListener {
public:
    StringValue(Schema::Setting &setting_, std::function<void(Schema::Setting &setting, const std::string &command)> write) : setting(setting_) {
        setReadOnly(setting.readOnly);
        setDefaultText(setting.emptyString);
        onReturnKey = onEscapeKey = onFocusLost = [&, write] {
            setting.clear();
            write(setting, setting.getWrite("\"" + getText().toStdString() + "\""));
        };

        setting.addChangeListener(this);
        changeListenerCallback({});
    }

    ~StringValue() override {
        setting.removeChangeListener(this);
    }

private:
    Schema::Setting &setting;

    void changeListenerCallback(juce::ChangeBroadcaster *) override {
        if (setting.status != Schema::Setting::Status::confirmed) {
            setText({}, false);
            return;
        }

        // TODO: Check length?
        setText(setting.value.substr(1, setting.value.size() - 2), false);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringValue)
};
