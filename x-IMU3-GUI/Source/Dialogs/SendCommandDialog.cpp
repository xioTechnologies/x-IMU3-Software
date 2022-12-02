#include "ApplicationSettingsDialog.h"
#include "SendCommandDialog.h"

SendCommandDialog::SendCommandDialog(const juce::String& title) : Dialog(BinaryData::json_svg, title, "Send")
{
    addAndMakeVisible(label);
    addAndMakeVisible(commandValue);

    commandValue.setEditableText(true);

    commandHistory = juce::ValueTree::fromXml(file.loadFileAsString());
    if (commandHistory.isValid() == false)
    {
        commandHistory = juce::ValueTree("CommandHistory");
    }
    for (const auto command : commandHistory)
    {
        commandValue.addItem(command.getProperty("command"), commandValue.getNumItems() + 1);
    }

    commandValue.setText(commandValue.getNumItems() > 0 ? commandValue.getItemText(0) : "{\"note\":\"Hello, World!\"}", juce::dontSendNotification);

    commandValue.onChange = [&]
    {
        setOkButton(commandValue.getText().isNotEmpty());
    };
    commandValue.onChange();

    setSize(dialogWidth, calculateHeight(1));
}

void SendCommandDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds();
    label.setBounds(bounds.removeFromLeft(columnWidth));
    commandValue.setBounds(bounds);
}

CommandMessage SendCommandDialog::getCommand()
{
    if (commandValue.getText().isNotEmpty())
    {
        for (const auto command : commandHistory)
        {
            if (command.getProperty("command") == commandValue.getText())
            {
                commandHistory.removeChild(command, nullptr);
                break;
            }
        }

        while (commandHistory.getNumChildren() > 9)
        {
            commandHistory.removeChild(commandHistory.getChild(commandHistory.getNumChildren() - 1), nullptr);
        }

        commandHistory.addChild({ "Command", {{ "command", commandValue.getText() }}}, 0, nullptr);
        file.replaceWithText(commandHistory.toXmlString());
    }

    return CommandMessage(commandValue.getText().toStdString());
}
