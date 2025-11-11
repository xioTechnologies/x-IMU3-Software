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
        value.onReturnKey = value.onEscapeKey = value.onFocusLost = [&]
        {
            if (value.getText() == getValueAsString())
            {
                return; // discard if the user hasn't changed text
            }

            juce::var newValue; // will be interpreted as null in json

            if (tree[DeviceSettingsIds::type] == "number")
            {
                try
                {
                    newValue = std::stof(value.getText().toStdString());
                }
                catch (...)
                {
                }
            }
            else
            {
                newValue = value.getText();
            }

            setValue(newValue);
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
        value.setDefaultText(tree.getProperty(DeviceSettingsIds::emptyValue, "Empty"));
    }

    juce::String getValueAsString() const
    {
        if (getValue().isDouble() && (std::abs((int) getValue()) >= 1000)) // do not use scientific notation
        {
            return juce::String((int) getValue());
        }
        return getValue();
    }

    void valueChanged() override
    {
        if (getValue().isVoid())
        {
            return;
        }

        updateTextToShowWhenEmpty();

        value.setText(getValueAsString(), juce::dontSendNotification);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingText)
};
