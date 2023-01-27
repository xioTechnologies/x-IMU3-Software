#include "ApplicationSettingsDialog.h"
#include "SendCommandDialog.h"

SendCommandDialog::SendCommandDialog(const juce::String& title) : Dialog(BinaryData::json_svg, title, "Send", "Cancel", &historyButton, 50)
{
    addAndMakeVisible(label);
    addAndMakeVisible(prefixLabel);
    addAndMakeVisible(commandKey);
    addAndMakeVisible(infixLabel);
    addAndMakeVisible(commandValue);
    addAndMakeVisible(postfixLabel);
    addAndMakeVisible(historyButton);

    commandHistory = juce::ValueTree::fromXml(file.loadFileAsString());
    if (! commandHistory.isValid())
    {
        commandHistory = juce::ValueTree("CommandHistory");
        commandHistory.appendChild({ "Command", {{ "key", "note" }, { "value", "\"Hello, World!\"" }}}, nullptr);
    }

    commandKey.setText(commandHistory.getChild(0)["key"], false);
    commandValue.setText(commandHistory.getChild(0)["value"], false);

    setSize(dialogWidth, calculateHeight(1));
}

void SendCommandDialog::resized()
{
    Dialog::resized();

    const auto prefixWidth = 30;

    auto bounds = getContentBounds();
    label.setBounds(bounds.removeFromLeft(columnWidth));
    prefixLabel.setBounds(bounds.removeFromLeft(prefixWidth));
    commandKey.setBounds(bounds.removeFromLeft(125));
    infixLabel.setBounds(bounds.removeFromLeft(prefixWidth));
    commandValue.setBounds(bounds.removeFromLeft(125));
    postfixLabel.setBounds(bounds.removeFromLeft(prefixWidth));
}

CommandMessage SendCommandDialog::getCommand()
{
    if (commandValue.getText().isNotEmpty())
    {
        for (const auto command : commandHistory)
        {
            if (command["key"] == commandKey.getText() && command["value"] == commandValue.getText())
            {
                commandHistory.removeChild(command, nullptr);
                break;
            }
        }

        while (commandHistory.getNumChildren() > 9)
        {
            commandHistory.removeChild(commandHistory.getChild(commandHistory.getNumChildren() - 1), nullptr);
        }

        commandHistory.addChild({ "Command", {{ "key", commandKey.getText() }, { "value", commandValue.getText() }}}, 0, nullptr);
        file.replaceWithText(commandHistory.toXmlString());
    }

    const auto json = "{" + commandKey.getText().quoted() + ":" + commandValue.getText() + "}";
    return CommandMessage { json.toStdString() };
}

juce::PopupMenu SendCommandDialog::getHistoryMenu()
{
    juce::PopupMenu menu;
    for (const auto child : commandHistory)
    {
        const auto json = "{" + child["key"].toString().quoted() + ":" + child["value"].toString() + "}";
        menu.addItem(json, [&, child]
        {
            commandKey.setText(child["key"], false);
            commandValue.setText(child["value"], false);
        });
    }
    return menu;
}
