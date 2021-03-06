#pragma once

#include "../DeviceSettingsIDs.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Setting.h"
#include "Widgets/CustomComboBox.h"

class SettingEnum : public Setting
{
public:
    SettingEnum(const juce::ValueTree& settingTree, const juce::ValueTree& typeTree) : SettingEnum(settingTree, [typeTree]
    {
        std::map<int, juce::String> choices;
        for (auto child : typeTree)
            choices[child.getProperty(DeviceSettingsIDs::value)] = child[DeviceSettingsIDs::displayName];
        return choices;
    }())
    {
    }

    SettingEnum(const juce::ValueTree& settingTree, const std::map<int, juce::String>& choices) : Setting(settingTree)
    {
        addAndMakeVisible(value);

        value.onChange = [&]
        {
            setValue(value.getSelectedId() - 1);
        };

        value.setEnabled(!isReadOnly());

        for (auto& choice : choices)
        {
            value.addItem(choice.second, choice.first + 1);
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
        const auto newValue = getValue();
        value.setSelectedId(newValue.isVoid() ? 0 : ((int) newValue + 1), juce::dontSendNotification);
    }

private:
    CustomComboBox value;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingEnum)
};
