#pragma once

#include "ApplicationSettings.h"
#include "Dialog.h"
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"

class SendCommandDialog : public Dialog {
public:
    explicit SendCommandDialog(const juce::String &dialogTitle, const std::optional<juce::Colour> &colourTag_ = {});

    void resized() override;

    std::string getCommand();

private:
    enum class Type {
        string,
        number,
        true_,
        false_,
        null,
    };

    static const inline std::map<Type, juce::String> typeStringMap
    {
        {Type::string, "string"},
        {Type::number, "number"},
        {Type::true_, "true"},
        {Type::false_, "false"},
        {Type::null, "null"},
    };

    SimpleLabel keyLabel{"Key:"};
    CustomTextEditor keyValue;
    IconButton dictionaryButton{BinaryData::dictionary_svg, "Dictionary", std::bind(&SendCommandDialog::getDictionaryMenu, this)};

    SimpleLabel valueLabel{"Value:"};
    CustomComboBox typeValue;
    CustomTextEditor stringValue;
    CustomTextEditor numberValue;

    SimpleLabel commandLabel{"Command:"};
    CustomTextEditor commandValue;

    IconButton previousCommandsButton{BinaryData::history_svg, "History", std::bind(&SendCommandDialog::getPreviousCommandsMenu, this)};

    const juce::ValueTree commandKeys = juce::ValueTree::fromXml(BinaryData::CommandKeys_xml);

    juce::ValueTree previousCommands;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Commands.xml");

    static Type typeFromString(const juce::String &string);

    static juce::String createCommand(const juce::String &key, const Type type, const juce::String &string, const juce::String &number);

    void selectCommand(const juce::ValueTree command);

    juce::PopupMenu getDictionaryMenu();

    juce::PopupMenu getPreviousCommandsMenu();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendCommandDialog)
};
