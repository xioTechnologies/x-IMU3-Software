#pragma once

#include "../ApplicationSettings.h"
#include "../CommandMessage.h"
#include "../Widgets/CustomComboBox.h"
#include "../Widgets/CustomTextEditor.h"
#include "../Widgets/SimpleLabel.h"
#include "Dialog.h"

class SendCommandDialog : public Dialog
{
public:
    explicit SendCommandDialog(const juce::String& title);

    void resized() override;

    CommandMessage getCommand();

private:
    SimpleLabel label { "Command:" };
    SimpleLabel prefixLabel { "{\"", UIFonts::getDefaultFont(), juce::Justification::centred };
    CustomTextEditor commandKey;
    SimpleLabel infixLabel { "\":", UIFonts::getDefaultFont(), juce::Justification::centred };
    CustomTextEditor commandValue;
    SimpleLabel postfixLabel { "}", UIFonts::getDefaultFont(), juce::Justification::centred };

    IconButton historyButton { IconButton::Style::menuStripDropdown, BinaryData::filter_svg, 0.8f, "Filter", std::bind(&SendCommandDialog::getHistoryMenu, this) };

    juce::ValueTree commandHistory;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Command History.xml");

    juce::PopupMenu getHistoryMenu();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendCommandDialog)
};
