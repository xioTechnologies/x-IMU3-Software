#include "ApplicationSettingsDialog.h"
#include "SendCommandDialog.h"

SendCommandDialog::SendCommandDialog(const juce::String& title) : Dialog(BinaryData::json_svg, title, "Send")
{
    addAndMakeVisible(label);
    addAndMakeVisible(commandValue);

    commandValue.setEditableText(true);

    recentCommands = juce::ValueTree::fromXml(file.loadFileAsString());
    if (recentCommands.isValid() == false)
    {
        recentCommands = juce::ValueTree("RecentCommands");
    }
    for (const auto recentCommand : recentCommands)
    {
        commandValue.addItem(recentCommand.getProperty("command"), commandValue.getNumItems() + 1);
    }

    commandValue.setText(commandValue.getNumItems() > 0 ? commandValue.getItemText(0) : "{\"note\":\"Hello, World!\"}", juce::dontSendNotification);

    commandValue.onChange = [&]
    {
        setValid(commandValue.getText().isNotEmpty());
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
        for (const auto recentCommand : recentCommands)
        {
            if (recentCommand.getProperty("command") == commandValue.getText())
            {
                recentCommands.removeChild(recentCommand, nullptr);
                break;
            }
        }

        while (recentCommands.getNumChildren() > 9)
        {
            recentCommands.removeChild(recentCommands.getChild(recentCommands.getNumChildren() - 1), nullptr);
        }

        recentCommands.addChild({ "Command", {{ "command", commandValue.getText() }}}, 0, nullptr);
        file.replaceWithText(recentCommands.toXmlString());
    }

    return CommandMessage(commandValue.getText().toStdString());
}
