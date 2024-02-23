#pragma once

#include "DeviceSettings/DeviceSettingsIDs.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Setting.h"
#include "Widgets/CustomComboBox.h"

class SettingEnum : public Setting
{
public:
    SettingEnum(const juce::ValueTree& settingTree, const juce::ValueTree& typeTree) : Setting(settingTree)
    {
        addAndMakeVisible(value);

        value.onChange = [&]
        {
            setValue(value.getSelectedId() - 1);
        };

        value.setEnabled(!isReadOnly());

        for (auto child : typeTree)
        {
            value.addItem(child[DeviceSettingsIDs::name], (int) child.getProperty(DeviceSettingsIDs::value) + 1);
        }

        valueChanged();
    }

    void resized() override
    {
        Setting::resized();
        value.setBounds(getValueBounds());
    }

protected:
    void valueChanged() override
    {
        int id = 0;
        if (getValue().has_value())
        {
            id = (int) *getValue() + 1;
        }
        value.setSelectedId(id, juce::dontSendNotification);
    }

private:
    CustomComboBox value;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingEnum)
};
