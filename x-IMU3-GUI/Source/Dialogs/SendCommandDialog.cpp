#include "Schema/Schema.h"
#include "SendCommandDialog.h"
#include "Widgets/PopupMenuHeader.h"

SendCommandDialog::Dictionary::Dictionary(const std::vector<ConnectionPanel *> &connectionPanels) {
    juce::StringArray missingNames;

    for (auto *connectionPanel: connectionPanels) {
        auto response = connectionPanel->getConnection()->getPingResponse();

        if (response.has_value() == false) {
            continue;
        }

        const juce::String model = response->device_name; // TODO: Use model

        const auto schema = Schema::find(model);

        if (schema.has_value() == false) {
            missingNames.addIfNotAlreadyThere(model);
            continue;
        }

        names.addIfNotAlreadyThere(schema->name);

        addCommands(schema->commands);
        addSettings(schema->settings);
    }

    commands.sort(false);
    settings.sort(false);

    if (names.isEmpty() == false) {
        return;
    }

    if (missingNames.isEmpty() == false) {
        error = "Schema not found for " + missingNames.joinIntoString(", ");
        return;
    }

    error = "No ping response";
}

void SendCommandDialog::Dictionary::addCommands(juce::ValueTree tree) {
    for (auto child: tree) {
        commands.addIfNotAlreadyThere(child.getProperty("key"));
    }
}

void SendCommandDialog::Dictionary::addSettings(juce::ValueTree tree) {
    for (auto child: tree) {
        if (child.hasProperty("key")) {
            settings.addIfNotAlreadyThere(child.getProperty("key"));
        }

        addSettings(child);
    }
}

SendCommandDialog::SendCommandDialog(const juce::String &dialogTitle, const Dictionary &dictionary_, const std::optional<juce::Colour> &colourTag_)
    : Dialog(BinaryData::json_svg, dialogTitle, "Send", "Cancel", &previousCommandsButton, iconButtonWidth, false, colourTag_),
      dictionary(dictionary_) {
    addAndMakeVisible(keyLabel);
    addAndMakeVisible(keyValue);
    addAndMakeVisible(dictionaryButton);
    addAndMakeVisible(valueLabel);
    addAndMakeVisible(typeValue);
    addAndMakeVisible(stringValue);
    addAndMakeVisible(numberRawValue);
    addAndMakeVisible(commandLabel);
    addAndMakeVisible(commandValue);
    addAndMakeVisible(previousCommandsButton);

    dictionaryButton.setEnabled(dictionary.error.has_value() == false);

    for (const auto child: juce::ValueTree::fromXml(file.loadFileAsString())) {
        if (const auto command = ximu3::CommandMessage::parse(child["json"].toString().toStdString())) {
            previousCommands.push_back(*command);
        }
    }

    if (previousCommands.empty()) {
        if (const auto command = ximu3::CommandMessage::parse("{\"ping\":null}")) {
            previousCommands.push_back(*command);
        }
    }

    typeValue.addItemList(typeStrings, 1);

    keyValue.onTextChange = typeValue.onChange = stringValue.onTextChange = numberRawValue.onTextChange = [&] {
        const auto type = static_cast<Type>(typeValue.getSelectedItemIndex());
        const auto value = [&]() -> juce::String {
            switch (type) {
                case Type::string:
                    return "\"" + stringValue.getText() + "\"";
                case Type::numberRaw:
                    return numberRawValue.getText();
                case Type::true_:
                case Type::false_:
                case Type::null:
                    return typeStrings[static_cast<int>(type)];
            }
            return ""; // avoid compiler warning
        }();
        commandValue.setText("{\"" + keyValue.getText() + "\":" + value + "}", false);
        stringValue.setVisible(type == Type::string);
        numberRawValue.setVisible(type == Type::numberRaw);

        setOkButton((keyValue.isEmpty() == false) && ximu3::CommandMessage::parse(commandValue.getText().toStdString()).has_value());
    };

    selectCommand(previousCommands.front());

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
    typeValue.setBounds(valueRow.removeFromLeft(125));
    valueRow.removeFromLeft(Dialog::margin);
    stringValue.setBounds(valueRow);
    numberRawValue.setBounds(valueRow);

    bounds.removeFromTop(Dialog::margin);

    auto commandRow = bounds.removeFromTop(UILayout::textComponentHeight);
    commandLabel.setBounds(commandRow.removeFromLeft(columnWidth));
    commandValue.setBounds(commandRow);
}

std::string SendCommandDialog::getCommand() {
    if (const auto command = ximu3::CommandMessage::parse(commandValue.getText().toStdString())) {
        std::erase_if(previousCommands, [&](const auto &previousCommand) {
            return previousCommand.json == command->json;
        });

        previousCommands.insert(previousCommands.begin(), *command);

        if (previousCommands.size() > 18) {
            previousCommands.resize(18);
        }

        juce::ValueTree tree("Commands");
        for (const auto &previousCommand: previousCommands) {
            tree.appendChild({"Command", {{"json", juce::String(previousCommand.json)}}}, nullptr);
        }
        file.replaceWithText(tree.toXmlString());
    }

    return commandValue.getText().toStdString();
}

void SendCommandDialog::selectCommand(const ximu3::CommandMessage &command) {
    keyValue.setText(command.key, false);
    stringValue.setText("", false);
    numberRawValue.setText("", false);

    switch (command.valueType) {
        case ximu3::XIMU3_JsonTypeString:
            typeValue.setSelectedItemIndex(static_cast<int>(Type::string), juce::dontSendNotification);
            stringValue.setText(command.value.substr(1, command.value.size() - 2), false);
            break;

        case ximu3::XIMU3_JsonTypeNumber:
        case ximu3::XIMU3_JsonTypeObject:
        case ximu3::XIMU3_JsonTypeArray:
            typeValue.setSelectedItemIndex(static_cast<int>(Type::numberRaw), juce::dontSendNotification);
            numberRawValue.setText(command.value, false);
            break;

        case ximu3::XIMU3_JsonTypeBoolean:
            typeValue.setSelectedItemIndex(static_cast<int>(command.value == "true" ? Type::true_ : Type::false_), juce::dontSendNotification);
            break;

        case ximu3::XIMU3_JsonTypeNull:
            typeValue.setSelectedItemIndex(static_cast<int>(Type::null), juce::dontSendNotification);
            break;
    }

    keyValue.onTextChange();
}

juce::PopupMenu SendCommandDialog::getDictionaryMenu() {
    juce::PopupMenu menu;

    const auto addItem = [&](const auto &key) {
        menu.addItem(key, [&, key] {
            keyValue.setText(key, juce::sendNotification);
            typeValue.setSelectedItemIndex(static_cast<int>(Type::null), juce::sendNotification);
            stringValue.setText({}, juce::sendNotification);
            numberRawValue.setText({}, juce::sendNotification);
        });
    };

    for (const auto &command: dictionary.commands) {
        addItem(command);
    }

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("DEVICE SETTINGS"), nullptr);

    for (const auto &setting: dictionary.settings) {
        addItem(setting);
    }

    return menu;
}

juce::PopupMenu SendCommandDialog::getPreviousCommandsMenu() {
    juce::PopupMenu menu;
    for (const auto &command: previousCommands) {
        menu.addItem(command.json, [&, command] {
            selectCommand(command);
        });
    }
    return menu;
}
