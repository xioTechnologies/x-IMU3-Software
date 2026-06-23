#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Schema.h"
#include "Value.h"
#include "Widgets/CustomComboBox.h"

class BooleanValue final : public CustomComboBox,
                           public Value {
public:
    BooleanValue(Schema::Setting &setting_, std::function<void(Schema::Setting &setting, const std::string &command)> write) : setting(setting_) {
        addItemList({"Disabled", "Enabled"}, 1);
        setEnabled(setting.readOnly == false);
        onChange = [&, write] {
            setting.clear();
            write(setting, setting.getWriteCommand(getSelectedItemIndex() == 1 ? "true" : "false"));
        };

        refresh();
    }

    void refresh() override {
        if (setting.status != Schema::Setting::Status::confirmed) {
            setSelectedId(0, juce::dontSendNotification);
            return;
        }

        setSelectedItemIndex(setting.value == "true" ? 1 : 0, juce::dontSendNotification);
    }

private:
    Schema::Setting &setting;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BooleanValue)
};
