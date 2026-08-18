#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Schema.h"
#include "Widgets/CustomComboBox.h"

class EnumerationValue : public CustomComboBox,
                  private juce::ChangeListener {
public:
    EnumerationValue(Schema::Setting &setting_, std::function<void(Schema::Setting &setting, const std::string &command)> write) : setting(setting_) {
        for (const auto &enumerator: setting.enumeration) {
            addItem(enumerator.second, 1 + getNumItems());
        }
        setEnabled(setting.readOnly == false);
        onChange = [&, write] {
            setting.clear();
            write(setting, setting.getWrite(std::to_string(setting.enumeration[(size_t) getSelectedItemIndex()].first)));
        };

        setting.addChangeListener(this);
        changeListenerCallback({});
    }

    ~EnumerationValue() override {
        setting.removeChangeListener(this);
    }

private:
    Schema::Setting &setting;

    void changeListenerCallback(juce::ChangeBroadcaster *) override {
        if (setting.status != Schema::Setting::Status::confirmed) {
            setSelectedId(0, juce::dontSendNotification);
            return;
        }

        for (size_t index = 0; index < setting.enumeration.size(); index++) {
            if (std::to_string(setting.enumeration[index].first) == setting.value) {
                setSelectedItemIndex((int) index, juce::dontSendNotification);
                return;
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnumerationValue)
};
