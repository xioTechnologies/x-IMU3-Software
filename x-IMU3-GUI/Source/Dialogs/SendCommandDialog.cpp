#include "SendCommandDialog.h"
#include "Widgets/PopupMenuHeader.h"

SendCommandDialog::SendCommandDialog(const juce::String &dialogTitle, const std::optional<juce::Colour> &colourTag_) : Dialog(BinaryData::json_svg, dialogTitle, "Send", "Cancel", &previousCommandsButton, iconButtonWidth, false, colourTag_) {
    addAndMakeVisible(keyLabel);
    addAndMakeVisible(keyValue);
    addAndMakeVisible(dictionaryButton);
    addAndMakeVisible(valueLabel);
    addAndMakeVisible(typeValue);
    addAndMakeVisible(stringValue);
    addAndMakeVisible(numberValue);
    addAndMakeVisible(commandLabel);
    addAndMakeVisible(commandValue);
    addAndMakeVisible(previousCommandsButton);

    previousCommands = juce::ValueTree::fromXml(file.loadFileAsString());
    if (!previousCommands.isValid()) {
        previousCommands = juce::ValueTree("Commands");
        previousCommands.appendChild({"Command", {{"key", "ping"}, {"type", typeStringMap.at(Type::null)}}}, nullptr);
    }

    typeValue.addItemList({typeStringMap.at(Type::string), typeStringMap.at(Type::number), typeStringMap.at(Type::true_), typeStringMap.at(Type::false_), typeStringMap.at(Type::null)}, 1);

    keyValue.onTextChange = typeValue.onChange = stringValue.onTextChange = numberValue.onTextChange = [&] {
        const auto type = static_cast<Type>(typeValue.getSelectedItemIndex());
        commandValue.setText(createCommand(keyValue.getText(), type, stringValue.getText(), numberValue.getText()), false);
        stringValue.setVisible(type == Type::string);
        numberValue.setVisible(type == Type::number);

        setOkButton((keyValue.isEmpty() == false) && ximu3::CommandMessage::parse(commandValue.getText().toStdString()).has_value());
    };

    selectCommand(previousCommands.getChild(0));

    commandValue.setReadOnly(true);

    previousCommandsButton.setWantsKeyboardFocus(false);

    setSize(600, calculateHeight(3));
}

void SendCommandDialog::resized() {
    Dialog::resized();

    auto bounds = getContentBounds();

    auto keyRow = bounds.removeFromTop(UILayout::textComponentHeight);
    keyLabel.setBounds(keyRow.removeFromLeft(columnWidth));
    dictionaryButton.setBounds(keyRow.removeFromRight(iconButtonWidth));
    keyValue.setBounds(keyRow.withTrimmedRight(margin));

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

std::string SendCommandDialog::getCommand() {
    juce::ValueTree newCommand{"Command", {{"key", keyValue.getText()}, {"type", typeStringMap.at(static_cast<Type>(typeValue.getSelectedItemIndex()))}}};
    switch (static_cast<Type>(typeValue.getSelectedItemIndex())) {
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

    for (const auto command: previousCommands) {
        if (command.isEquivalentTo(newCommand)) {
            previousCommands.removeChild(command, nullptr);
            break;
        }
    }

    while (previousCommands.getNumChildren() >= 18) {
        previousCommands.removeChild(previousCommands.getChild(previousCommands.getNumChildren() - 1), nullptr);
    }

    previousCommands.addChild(newCommand, 0, nullptr);
    file.replaceWithText(previousCommands.toXmlString());

    return commandValue.getText().toStdString();
}

SendCommandDialog::Type SendCommandDialog::typeFromString(const juce::String &string) {
    for (const auto &it: typeStringMap) {
        if (it.second == string) {
            return it.first;
        }
    }

    return Type::null;
}

juce::String SendCommandDialog::createCommand(const juce::String &key, const Type type, const juce::String &string, const juce::String &number) {
    juce::String text = "{\"" + key + "\":";
    switch (type) {
        case Type::string:
            text += "\"" + string + "\"";
            break;
        case Type::number:
            text += number;
            break;
        case Type::true_:
        case Type::false_:
        case Type::null:
            text += typeStringMap.at(type);
            break;
    }
    return text + "}";
}

void SendCommandDialog::selectCommand(const juce::ValueTree command) {
    keyValue.setText(command["key"], false);
    typeValue.setSelectedItemIndex(static_cast<int>(typeFromString(command["type"])), juce::dontSendNotification);
    stringValue.setText(command["type"] == typeStringMap.at(Type::string) ? command["value"] : "", false);
    numberValue.setText(command["type"] == typeStringMap.at(Type::number) ? command["value"] : "", false);
    keyValue.onTextChange();
}

juce::PopupMenu SendCommandDialog::getDictionaryMenu() {
    juce::PopupMenu menu;
    for (const auto command: commandKeys) {
        if (command.hasType("Command")) {
            menu.addItem(command["key"], [&, command] {
                keyValue.setText(command["key"], juce::sendNotification);
                typeValue.setSelectedItemIndex(static_cast<int>(typeFromString(command["type"])), juce::sendNotification);
                stringValue.setText({}, juce::sendNotification);
                numberValue.setText({}, juce::sendNotification);
            });
        } else if (command.hasType("Separator")) {
            menu.addSeparator();
            menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>(command["header"]), nullptr);
        }
    }
    return menu;
}

juce::PopupMenu SendCommandDialog::getPreviousCommandsMenu() {
    juce::PopupMenu menu;
    for (const auto command: previousCommands) {
        menu.addItem(createCommand(command["key"], typeFromString(command["type"]), command["value"], command["value"]), [&, command] {
            selectCommand(command);
        });
    }
    return menu;
}
