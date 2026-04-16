#pragma once

#include "DeviceSettings/DeviceSettingsIds.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Setting.h"
#include "Widgets/CustomComboBox.h"

class SettingEnum : public Setting {
public:
    SettingEnum(const juce::ValueTree &settingTree, const juce::ValueTree &typeTree_) : Setting(settingTree), typeTree(typeTree_) {
        addAndMakeVisible(value);

        value.onChange = [&] {
            const auto var = typeTree.getChild(value.getSelectedItemIndex())[DeviceSettingsIds::value];
            setValue(isToggle() ? var : juce::var(static_cast<int>(var)));
        };

        value.setEnabled(!isReadOnly());

        for (auto child: typeTree) {
            value.addItem(child[DeviceSettingsIds::name], 1 + value.getNumItems());
        }

        valueChanged();
    }

    void resized() override {
        Setting::resized();
        value.setBounds(getValueBounds());
    }

protected:
    void valueChanged() override {
        for (int index = 0; index < typeTree.getNumChildren(); index++)
        {
            if (getValue() == typeTree.getChild(index)[DeviceSettingsIds::value])
            {
                value.setSelectedItemIndex(index, juce::dontSendNotification);
                return;
            }
        }

        value.setSelectedId(0, juce::dontSendNotification);
    }

    virtual bool isToggle() const {
        return false;
    }

private:
    const juce::ValueTree typeTree;

    CustomComboBox value;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingEnum)
};
