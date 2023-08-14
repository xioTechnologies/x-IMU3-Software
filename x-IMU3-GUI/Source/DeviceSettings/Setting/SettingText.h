#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Setting.h"
#include "Widgets/CustomTextEditor.h"

class SettingText : public Setting
{
public:
    explicit SettingText(const juce::ValueTree& settingTree) : Setting(settingTree)
    {
        addAndMakeVisible(value);
        value.onTextChange = [&]
        {
            if (tree[DeviceSettingsIDs::type] == "number")
            {
                try
                {
                    setValue(std::stof(value.getText().toStdString()));
                }
                catch (...)
                {
                    setValue({}); // will be interpreted as null in json
                }
            }
            else
            {
                setValue(value.getText());
            }

            updateTextToShowWhenEmpty();
        };
        value.setReadOnly(isReadOnly());

        valueChanged();
    }

    void resized() override
    {
        Setting::resized();
        value.setBounds(getValueBounds());
    }

protected:
    CustomTextEditor value;

    void updateTextToShowWhenEmpty()
    {
        value.setTextToShowWhenEmpty(tree.getProperty(DeviceSettingsIDs::emptyValue, "Empty"), juce::Colours::grey);
    }

    void valueChanged() override
    {
        if (getValue().has_value() == false)
        {
            return;
        }

        updateTextToShowWhenEmpty();

        if (getValue()->isDouble() && (std::abs((int) *getValue()) >= 1000)) // do not use scientific notation
        {
            value.setText(juce::String((int) *getValue()), juce::dontSendNotification);
        }
        else
        {
            value.setText(*getValue(), juce::dontSendNotification);
        }
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingText)
};
