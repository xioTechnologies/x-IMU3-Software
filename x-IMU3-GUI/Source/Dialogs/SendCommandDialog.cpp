#include "SendCommandDialog.h"

SendCommandDialog::SendCommandDialog(const juce::String& title) : Dialog(BinaryData::json_svg, title, "Send", "Cancel", &historyButton, 60)
{
    addAndMakeVisible(keyLabel);
    addAndMakeVisible(valueLabel);
    addAndMakeVisible(commandLabel);
    addAndMakeVisible(keyValue);
    addAndMakeVisible(typeValue);
    addAndMakeVisible(stringValue);
    addAndMakeVisible(numberValue);
    addAndMakeVisible(commandValue);
    addAndMakeVisible(historyButton);

    commandHistory = juce::ValueTree::fromXml(file.loadFileAsString());
    if (!commandHistory.isValid())
    {
        commandHistory = juce::ValueTree("CommandHistory");
        commandHistory.appendChild({ "Command", {{ "key", "note" }, { "type", static_cast<int>(Type::string) }, { "value", "Hello, World!" }}}, nullptr);
    }

    typeValue.addItemList({ toString(Type::string), toString(Type::number), toString(Type::true_), toString(Type::false_), toString(Type::null) }, 1);

    keyValue.onTextChange = typeValue.onChange = stringValue.onTextChange = numberValue.onTextChange = [&]
    {
        const auto type = static_cast<Type>(typeValue.getSelectedItemIndex());
        commandValue.setText(createCommand(keyValue.getText(), type, stringValue.getText(), numberValue.getText()), false);
        stringValue.setVisible(type == Type::string);
        numberValue.setVisible(type == Type::number);
    };

    selectCommand(commandHistory.getChild(0));

    commandValue.setReadOnly(true);

    historyButton.setWantsKeyboardFocus(false);

    setSize(dialogWidth, calculateHeight(3));
}

void SendCommandDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds();

    auto keyRow = bounds.removeFromTop(UILayout::textComponentHeight);
    keyLabel.setBounds(keyRow.removeFromLeft(columnWidth));
    keyValue.setBounds(keyRow);

    bounds.removeFromTop(Dialog::margin);

    auto valueRow = bounds.removeFromTop(UILayout::textComponentHeight);
    valueLabel.setBounds(valueRow.removeFromLeft(columnWidth));
    typeValue.setBounds(valueRow.removeFromLeft(columnWidth));
    valueRow.removeFromLeft(Dialog::margin);
    stringValue.setBounds(valueRow);
    numberValue.setBounds(valueRow);

    bounds.removeFromTop(Dialog::margin);

    auto commandRow = bounds.removeFromTop(UILayout::textComponentHeight);
    commandLabel.setBounds(commandRow.removeFromLeft(columnWidth));
    commandValue.setBounds(commandRow);
}

CommandMessage SendCommandDialog::getCommand()
{
    juce::ValueTree newCommand { "Command", {{ "key", keyValue.getText() }, { "type", typeValue.getSelectedItemIndex() }}};
    switch (static_cast<Type>(typeValue.getSelectedItemIndex()))
    {
        case Type::string:
            newCommand.setProperty("value", stringValue.getText(), nullptr);
            break;

        case Type::number:
            newCommand.setProperty("value", numberValue.getText(), nullptr);
            break;

        case Type::true_:
        case Type::false_:
        case Type::null:
            break;
    }

    for (const auto command : commandHistory)
    {
        if (command.isEquivalentTo(newCommand))
        {
            commandHistory.removeChild(command, nullptr);
            break;
        }
    }

    while (commandHistory.getNumChildren() > 9)
    {
        commandHistory.removeChild(commandHistory.getChild(commandHistory.getNumChildren() - 1), nullptr);
    }

    commandHistory.addChild(newCommand, 0, nullptr);
    file.replaceWithText(commandHistory.toXmlString());

    return CommandMessage { commandValue.getText().toStdString() };
}

juce::String SendCommandDialog::toString(const Type type)
{
    switch (type)
    {
        case Type::string:
            return "string";
        case Type::number:
            return "number";
        case Type::true_:
            return "true";
        case Type::false_:
            return "false";
        case Type::null:
            return "null";
    }
    return ""; // avoid compiler warning
}

juce::String SendCommandDialog::createCommand(const juce::String& key, const Type type, const juce::String& string, const juce::String& number)
{
    juce::String text = "{" + key.quoted() + ":";
    switch (type)
    {
        case Type::string:
            text += string.quoted();
            break;
        case Type::number:
            text += number;
            break;
        case Type::true_:
        case Type::false_:
        case Type::null:
            text += toString(type);
            break;
    }
    return text + "}";
}

void SendCommandDialog::selectCommand(const juce::ValueTree command)
{
    keyValue.setText(command["key"], false);
    typeValue.setSelectedItemIndex(command["type"], juce::dontSendNotification);
    stringValue.setText(static_cast<Type>((int) command["type"]) == Type::string ? command["value"] : "", false);
    numberValue.setText(static_cast<Type>((int) command["type"]) == Type::number ? command["value"] : "", false);
    keyValue.onTextChange();
}

juce::PopupMenu SendCommandDialog::getHistoryMenu()
{
    juce::PopupMenu menu;
    for (const auto child : commandHistory)
    {
        menu.addItem(createCommand(child["key"], static_cast<Type>((int) child["type"]), child["value"], child["value"]), [&, child]
        {
            selectCommand(child);
        });
    }
    return menu;
}
