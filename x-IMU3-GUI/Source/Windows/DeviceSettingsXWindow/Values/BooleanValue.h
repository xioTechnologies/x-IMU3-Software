#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Schema.h"
#include "Widgets/CustomComboBox.h"

class BooleanValue : public CustomComboBox,
                     private juce::ChangeListener {
public:
    BooleanValue(Schema::Setting &setting_, std::function<void(Schema::Setting &setting, const std::string &command)> write) : setting(setting_) {
        addItemList({"Disabled", "Enabled"}, 1);
        setEnabled(setting.readOnly == false);
        onChange = [&, write] {
            setting.clear();
            write(setting, setting.getWrite(getSelectedItemIndex() == 1 ? "true" : "false"));
        };

        setting.addChangeListener(this);
        changeListenerCallback({});
    }

    ~BooleanValue() override {
        setting.removeChangeListener(this);
    }

private:
    Schema::Setting &setting;

    void changeListenerCallback(juce::ChangeBroadcaster *) override {
        if (setting.status != Schema::Setting::Status::confirmed) {
            setSelectedId(0, juce::dontSendNotification);
            return;
        }

        setSelectedItemIndex(setting.value == "true" ? 1 : 0, juce::dontSendNotification);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BooleanValue)
};
