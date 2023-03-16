#pragma once

#include "../ApplicationSettings.h"
#include "../CommandMessage.h"
#include "../Widgets/CustomComboBox.h"
#include "../Widgets/CustomTextEditor.h"
#include "../Widgets/IconButton.h"
#include "../Widgets/SimpleLabel.h"
#include "Dialog.h"

class SendCommandDialog : public Dialog
{
public:
    explicit SendCommandDialog(const juce::String& title);

    void resized() override;

    CommandMessage getCommand();

private:
    enum class Type
    {
        string,
        number,
        true_,
        false_,
        null,
    };

    SimpleLabel keyLabel { "Key:" };
    CustomTextEditor keyValue;
    IconButton commandKeys { IconButton::Style::menuStripDropdown, BinaryData::search_svg, 1.0f, "Command Keys" };

    SimpleLabel valueLabel { "Value:" };
    CustomComboBox typeValue;
    CustomTextEditor stringValue;
    CustomTextEditor numberValue;

    SimpleLabel commandLabel { "Command:" };
    CustomTextEditor commandValue;

    IconButton historyButton { IconButton::Style::menuStripDropdown, BinaryData::default_svg, 0.8f, "Command History", std::bind(&SendCommandDialog::getHistoryMenu, this) };

    const juce::ValueTree keysTree = juce::ValueTree::fromXml(BinaryData::CommandKeys_xml);

    juce::ValueTree commandHistory;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Command History.xml");

    static juce::String toString(const Type type);

    static juce::String createCommand(const juce::String& key, const Type type, const juce::String& string, const juce::String& number);

    void selectCommand(const juce::ValueTree command);

    juce::PopupMenu getHistoryMenu();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendCommandDialog)
};
