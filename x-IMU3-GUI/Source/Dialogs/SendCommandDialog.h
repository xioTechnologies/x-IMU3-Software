#pragma once

#include "ApplicationSettings.h"
#include "CommandMessage.h"
#include "Dialog.h"
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"

class SendCommandDialog : public Dialog
{
public:
    explicit SendCommandDialog(const juce::String& title, const std::optional<juce::Colour>& tag_ = {});

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
    IconButton commandKeysButton { BinaryData::dictionary_svg, "Command Keys", std::bind(&SendCommandDialog::getCommandKeysMenu, this) };

    SimpleLabel valueLabel { "Value:" };
    CustomComboBox typeValue;
    CustomTextEditor stringValue;
    CustomTextEditor numberValue;

    SimpleLabel commandLabel { "Command:" };
    CustomTextEditor commandValue;

    IconButton recentCommandsButton { BinaryData::history_svg, "Recent Commands", std::bind(&SendCommandDialog::getRecentCommandsMenu, this) };

    const juce::ValueTree commandKeys = juce::ValueTree::fromXml(BinaryData::CommandKeys_xml);

    juce::ValueTree recentCommands;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Recent Commands.xml");

    static juce::String toString(const Type type);

    static juce::String createCommand(const juce::String& key, const Type type, const juce::String& string, const juce::String& number);

    void selectCommand(const juce::ValueTree command);

    juce::PopupMenu getCommandKeysMenu();

    juce::PopupMenu getRecentCommandsMenu();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendCommandDialog)
};
