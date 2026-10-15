#pragma once

#include "ApplicationSettings.h"
#include "ConnectionPanel/ConnectionPanel.h"
#include "Dialog.h"
#include "Schema/Schema.h"
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"

class SendCommandDialog : public Dialog {
public:
    class Dictionary {
    public:
        explicit Dictionary(const std::vector<ConnectionPanel *> &connectionPanels);

        juce::StringArray names;
        juce::StringArray commands;
        juce::StringArray settings;
        std::optional<juce::String> error;

    private:
        void addCommands(juce::ValueTree tree);

        void addSettings(juce::ValueTree tree);
    };

    explicit SendCommandDialog(const juce::String &dialogTitle, const Dictionary &dictionary_, const std::optional<juce::Colour> &colourTag_ = {});

    void resized() override;

    std::string getCommand();

private:
    enum class Type {
        string,
        numberRaw,
        true_,
        false_,
        null,
    };

    static const inline juce::StringArray typeStrings{"String", "Number/Raw", "true", "false", "null"};

    const Dictionary dictionary;

    SimpleLabel keyLabel{"Key:"};
    CustomTextEditor keyValue;
    IconButton dictionaryButton{BinaryData::dictionary_svg, "Dictionary: " + (dictionary.error ? *dictionary.error : dictionary.names.joinIntoString(", ")), std::bind(&SendCommandDialog::getDictionaryMenu, this)};

    SimpleLabel valueLabel{"Value:"};
    CustomComboBox typeValue;
    CustomTextEditor stringValue;
    CustomTextEditor numberRawValue;

    SimpleLabel commandLabel{"Command:"};
    CustomTextEditor commandValue;

    IconButton previousCommandsButton{BinaryData::history_svg, "History", std::bind(&SendCommandDialog::getPreviousCommandsMenu, this)};

    std::vector<ximu3::CommandMessage> previousCommands;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Commands.xml");

    void selectCommand(const ximu3::CommandMessage &command);

    juce::PopupMenu getDictionaryMenu();

    juce::PopupMenu getPreviousCommandsMenu();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendCommandDialog)
};
