#pragma once

#include "../ApplicationSettings.h"
#include "../Widgets/CustomComboBox.h"
#include "../Widgets/SimpleLabel.h"
#include "Dialog.h"

class SendCommandDialog : public Dialog
{
public:
    explicit SendCommandDialog(const juce::String& title);

    void resized() override;

    std::string getCommand();

private:
    SimpleLabel label { "Command:" };
    CustomComboBox commandValue;

    juce::ValueTree recentCommands;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Recent Commands.xml");

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendCommandDialog)
};
