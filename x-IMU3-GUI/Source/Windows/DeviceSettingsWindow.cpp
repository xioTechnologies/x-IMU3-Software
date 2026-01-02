#include "ConnectionPanel/ConnectionPanel.h"
#include "DeviceSettingsWindow.h"
#include "Dialogs/MessageDialog.h"
#include "Firmware/Firmware.h"
#include "KeyCompare.h"
#include <ranges>
#include "Widgets/PopupMenuHeader.h"

DeviceSettingsWindow::DeviceSettingsWindow(const juce::ValueTree &windowLayout_, const juce::Identifier &type_, ConnectionPanel &connectionPanel_)
    : Window(windowLayout_, type_, connectionPanel_, "Device Settings Menu") {
    addAndMakeVisible(readAllButton);
    addAndMakeVisible(writeAllButton);
    addAndMakeVisible(saveToFileButton);
    addAndMakeVisible(loadFromFileButton);
    addAndMakeVisible(defaultsButton);
    addChildComponent(disabledOverlay);

    readAllButton.onClick = [this] {
        enableInProgress(deviceSettings->getReadKeys());

        connectionPanel.sendCommands(deviceSettings->getReadCommands(), this, [&, keys = deviceSettings->getReadKeys()](const auto &responses) {
            for (size_t index = 0; index < responses.size(); index++) {
                const auto key = keys[index];
                const auto &response = responses[index];

                if (response.has_value() == false || response->error.has_value()) {
                    deviceSettings->setStatus(key, Setting::Status::warning, "Unable to Read from Device");
                    continue;
                }

                deviceSettings->setValue(key, DeviceSettings::valueToVar(response->value));

                if (KeyCompare::compare(key, "firmware_version") && response->value != ("\"" + Firmware::version + "\"")) {
                    deviceSettings->setStatus(key, Setting::Status::warning, "Unexpected Firmware Version");
                    continue;
                }

                deviceSettings->setStatus(response->key, Setting::Status::normal, {});
            }

            readAllButton.setToggleState(keys.size() != responses.size(), juce::dontSendNotification);

            disableInProgress();
        });
    };

    writeAllButton.onClick = [this] {
        writeCommands(deviceSettings->getWriteKeys(), deviceSettings->getWriteCommands(true));
    };

    directory.createDirectory();

    saveToFileButton.onClick = [&] {
        auto fileName = deviceSettings->getValue("device_name").toString();
        if (fileName.isEmpty()) {
            fileName = "Unknown Device";
        }
        if (const auto serialNumber = deviceSettings->getValue("serial_number").toString(); serialNumber.isNotEmpty()) {
            fileName += " " + serialNumber;
        }

        fileChooser = std::make_unique<juce::FileChooser>("Save Device Settings", directory.getChildFile(fileName), "*.json");
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [&](const auto &) {
            if (fileChooser->getResult() == juce::File()) {
                return;
            }

            const auto keys = deviceSettings->getWriteKeys();
            const auto values = deviceSettings->getWriteValues(false);
            jassert(keys.size() == values.size());

            juce::DynamicObject object;
            for (size_t index = 0; index < keys.size(); index++) {
                object.setProperty(juce::String(keys[index]), juce::String(values[index]));
            }

            juce::FileOutputStream stream(fileChooser->getResult());
            stream.setPosition(0);
            stream.truncate();
            object.writeAsJSON(stream, {});
        });
    };

    loadFromFileButton.onClick = [&] {
        std::optional<juce::File> defaultFile;
        for (auto file: directory.findChildFiles(juce::File::findFiles, false, "*.json")) {
            if (file.getFileName().contains(deviceSettings->getValue("serial_number").toString()) && (file.getLastModificationTime() > defaultFile.value_or(juce::File()).getLastModificationTime())) {
                defaultFile = file;
            }
        }

        fileChooser = std::make_unique<juce::FileChooser>("Load Device Settings", defaultFile.value_or(directory), "*.json");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [&](const auto &) {
            if (fileChooser->getResult() == juce::File()) {
                return;
            }

            const auto parsed = juce::JSON::parse(fileChooser->getResult().loadFileAsString());
            if (auto *const object = parsed.getDynamicObject()) {
                for (const auto &command: object->getProperties()) {
                    deviceSettings->setValue(command.name.toString().toStdString(), command.value);
                }
            }

            if (readFromValueTree().writeSettingsWhenModified) {
                writeCommands(deviceSettings->getWriteKeys(), deviceSettings->getWriteCommands(true));
            }
        });
    };

    defaultsButton.onClick = [this] {
        DialogQueue::getSingleton().pushFront(std::make_unique<AreYouSureDialog>("Are you sure you want to restore default device settings?"), [this] {
            enableInProgress(deviceSettings->getReadCommands());

            sendCommand("default", false, [this] {
                sendCommand("save", false, [this] {
                    sendCommand("apply", true, [this] {
                        disableInProgress();
                        readAllButton.onClick();
                    });
                });
            });
            return true;
        });
    };

    valueTreePropertyChanged(settingsTree, "customSchema");

    setOpaque(true);
}

void DeviceSettingsWindow::paint(juce::Graphics &g) {
    g.fillAll(UIColours::backgroundLight);

    g.setColour(UIColours::backgroundDark);
    g.fillRect(buttonBounds);
}

void DeviceSettingsWindow::resized() {
    Window::resized();

    auto bounds = getContentBounds();
    disabledOverlay.setBounds(bounds);

    buttonBounds = bounds.removeFromBottom(25).toFloat();
    const auto buttonWidth = buttonBounds.getWidth() / buttons.size();

    deviceSettings->setBounds(bounds);
    juce::ScopedValueSetter _(buttonBounds, buttonBounds);
    for (auto *const button: buttons) {
        button->setBounds(buttonBounds.removeFromLeft(buttonWidth).reduced(2.0f).toNearestInt());
    }
}

void DeviceSettingsWindow::expandOrCollapseAll(juce::TreeViewItem &rootItem, const bool expand) {
    rootItem.setOpen(expand || rootItem.getParentItem() == nullptr);
    for (int index = 0; index < rootItem.getNumSubItems(); index++) {
        expandOrCollapseAll(*rootItem.getSubItem(index), expand);
    }
}

void DeviceSettingsWindow::writeToValueTree(const Settings &settings) {
    settingsTree.setProperty("hideUnusedSettings", settings.hideUnusedSettings, nullptr);
    settingsTree.setProperty("readSettingsWhenWindowOpens", settings.readSettingsWhenWindowOpens, nullptr);
    settingsTree.setProperty("writeSettingsWhenModified", settings.writeSettingsWhenModified, nullptr);
    if (settingsTree.getProperty("customSchema") != settings.customSchema.getFullPathName()) // avoid callback when property doesn't exist
    {
        settingsTree.setProperty("customSchema", settings.customSchema.getFullPathName(), nullptr);
    }
}

DeviceSettingsWindow::Settings DeviceSettingsWindow::readFromValueTree() {
    Settings settings;
    settings.hideUnusedSettings = settingsTree.getProperty("hideUnusedSettings", settings.hideUnusedSettings);
    settings.readSettingsWhenWindowOpens = settingsTree.getProperty("readSettingsWhenWindowOpens", settings.readSettingsWhenWindowOpens);
    settings.writeSettingsWhenModified = settingsTree.getProperty("writeSettingsWhenModified", settings.writeSettingsWhenModified);
    settings.customSchema = settingsTree.getProperty("customSchema");
    return settings;
}

void DeviceSettingsWindow::sendCommand(const juce::String &key, const bool silent, std::function<void()> callback) {
    const std::string command = "{\"" + key.toStdString() + "\":null}";

    connectionPanel.sendCommands({command}, this, [command, silent, callback](const auto &responses) {
        if (silent == false) {
            const auto showError = [command](const std::string &error) {
                DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>(command + " command failed. " + error + (error.ends_with(".") ? "" : ".")));
            };

            if (responses.front().has_value() == false) {
                showError("No response");
            } else if (responses.front()->error.has_value()) {
                showError(*responses.front()->error);
            }
        }

        callback();
    });
}

void DeviceSettingsWindow::writeCommands(const std::vector<std::string> &keys, const std::vector<std::string> &commands) {
    enableInProgress(keys);

    connectionPanel.sendCommands(commands, this, [&, keys, commands](const auto &responses) {
        for (size_t index = 0; index < responses.size(); index++) {
            const auto key = keys[index];
            const auto &response = responses[index];

            if (response.has_value() == false || response->error.has_value()) {
                deviceSettings->setStatus(key, Setting::Status::warning, "Unable to Write to Device");
                continue;
            }

            deviceSettings->setValue(response->key, DeviceSettings::valueToVar(response->value));
            deviceSettings->setStatus(key, Setting::Status::normal, {});
        }

        writeAllButton.setToggleState(commands.size() != responses.size(), juce::dontSendNotification);

        sendCommand("save", false, [this] {
            sendCommand("apply", true, [this] {
                disableInProgress();
            });
        });
    });
}

void DeviceSettingsWindow::enableInProgress(const std::vector<std::string> &keys) {
    for (const auto &key: keys) {
        deviceSettings->setStatus(key, Setting::Status::normal, {});
    }
    readAllButton.setToggleState(false, juce::dontSendNotification);
    writeAllButton.setToggleState(false, juce::dontSendNotification);

    disabledOverlay.setVisible(true);
}

void DeviceSettingsWindow::disableInProgress() {
    juce::Timer::callAfterDelay(100, [&, self = SafePointer<juce::Component>(this)] {
        if (self != nullptr) {
            disabledOverlay.setVisible(false);
        }
    });
}

juce::PopupMenu DeviceSettingsWindow::getMenu() {
    juce::PopupMenu menu = Window::getMenu();

    menu.addItem("Read Settings When Window Opens", true, readFromValueTree().readSettingsWhenWindowOpens, [&] {
        auto settings = readFromValueTree();
        settings.readSettingsWhenWindowOpens = !settings.readSettingsWhenWindowOpens;
        writeToValueTree(settings);
    });
    menu.addItem("Write Settings When Modified", true, readFromValueTree().writeSettingsWhenModified, [&] {
        auto settings = readFromValueTree();
        settings.writeSettingsWhenModified = !settings.writeSettingsWhenModified;
        writeToValueTree(settings);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("VIEW"), nullptr);
    menu.addItem("Hide Unused Settings", true, readFromValueTree().hideUnusedSettings, [&] {
        auto settings = readFromValueTree();
        settings.hideUnusedSettings = !settings.hideUnusedSettings;
        writeToValueTree(settings);
    });
    menu.addItem("Expand All", [&] {
        expandOrCollapseAll(*deviceSettings->getRootItem(), true);
    });
    menu.addItem("Collapse All", [&] {
        expandOrCollapseAll(*deviceSettings->getRootItem(), false);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("SCHEMA"), nullptr);
    menu.addItem("x-IMU3", true, readFromValueTree().customSchema == juce::File(), [&] {
        auto settings = readFromValueTree();
        settings.customSchema = juce::File();
        writeToValueTree(settings);
    });
    juce::PopupMenu customSchemasMenu;
    customSchemasMenu.addItem("Load .xml File", [&] {
        fileChooser = std::make_unique<juce::FileChooser>("Select Schema", juce::File(), "*.xml");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [&](const auto &) {
            if (fileChooser->getResult() == juce::File()) {
                return;
            }

            const auto customSchema = customSchemasDirectory.getChildFile(fileChooser->getResult().getFileName());
            customSchemasDirectory.createDirectory();
            fileChooser->getResult().copyFileTo(customSchema);

            if (readFromValueTree().customSchema == customSchema) {
                settingsTree.sendPropertyChangeMessage("customSchema");
            } else {
                auto settings = readFromValueTree();
                settings.customSchema = customSchema;
                writeToValueTree(settings);
            }
        });
    });
    if (const auto files = customSchemasDirectory.findChildFiles(juce::File::findFiles, false, "*.xml"); files.isEmpty() == false) {
        customSchemasMenu.addSeparator();
        customSchemasMenu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("PREVIOUS"), nullptr);
        for (const auto &file: files) {
            const auto ticked = readFromValueTree().customSchema == file.getFullPathName();
            customSchemasMenu.addItem(file.getFileNameWithoutExtension(), true, ticked, [&, file] {
                auto settings = readFromValueTree();
                settings.customSchema = file;
                writeToValueTree(settings);
            });
        }
    }

    const auto suffix = readFromValueTree().customSchema != juce::File() ? (" (" + juce::File(readFromValueTree().customSchema).getFileNameWithoutExtension() + ")") : "";
    menu.addSubMenu("Custom" + suffix, customSchemasMenu, true, nullptr, readFromValueTree().customSchema != juce::File());

    return menu;
}

void DeviceSettingsWindow::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) {
    if (treeWhosePropertyHasChanged != settingsTree) {
        return;
    }

    if (property.toString() == "customSchema") {
        const auto tree = [&] {
            if (const auto customSchemaFile = readFromValueTree().customSchema; customSchemaFile != juce::File()) {
                return juce::ValueTree::fromXml(juce::File(customSchemaFile).loadFileAsString());
            }
            return juce::ValueTree::fromXml(BinaryData::DeviceSettings_xml);
        }();

        deviceSettings = std::make_unique<DeviceSettings>(tree, [this](const std::string &key, const std::string &command) {
            if (readFromValueTree().writeSettingsWhenModified) {
                writeCommands({key}, {command});
            }
        });
        addAndMakeVisible(*deviceSettings);
        resized();

        if (readFromValueTree().readSettingsWhenWindowOpens) {
            readAllButton.onClick();
        }
    }

    if (deviceSettings) {
        deviceSettings->setHideUnusedSettings(readFromValueTree().hideUnusedSettings);
    }
}
