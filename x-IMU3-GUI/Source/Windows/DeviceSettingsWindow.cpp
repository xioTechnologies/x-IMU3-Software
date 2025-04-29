#include "ConnectionPanel/ConnectionPanel.h"
#include "DeviceSettingsWindow.h"
#include "Dialogs/MessageDialog.h"
#include "Firmware/Firmware.h"
#include "Widgets/PopupMenuHeader.h"

DeviceSettingsWindow::DeviceSettingsWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_)
    : Window(windowLayout_, type_, connectionPanel_, "Device Settings Menu")
{
    addAndMakeVisible(readAllButton);
    addAndMakeVisible(writeAllButton);
    addAndMakeVisible(saveToFileButton);
    addAndMakeVisible(loadFromFileButton);
    addAndMakeVisible(defaultsButton);
    addChildComponent(disabledOverlay);

    readAllButton.onClick = [this]
    {
        const auto commands = deviceSettings->getReadCommands();

        enableInProgress(commands);

        connectionPanel.sendCommands(commands, this, [&, commands](const std::vector<CommandMessage>& responses)
        {
            for (const auto& command : commands)
            {
                const auto response = std::find(responses.begin(), responses.end(), command);

                if (response == responses.end() || response->error)
                {
                    deviceSettings->setStatus(command.key, Setting::Status::warning, "Unable to Read from Device");
                    continue;
                }

                deviceSettings->setValue(*response);

                if (CommandMessage::normaliseKey(command.key) == CommandMessage::normaliseKey("firmware_version") && response->getValue() != Firmware::version)
                {
                    deviceSettings->setStatus(command.key, Setting::Status::warning, "Unexpected Firmware Version");
                    continue;
                }

                deviceSettings->setStatus(response->key, Setting::Status::normal, {});
            }

            readAllButton.setToggleState(commands.size() != responses.size(), juce::dontSendNotification);

            disableInProgress();
        });
    };

    writeAllButton.onClick = [this]
    {
        writeCommands(deviceSettings->getWriteCommands(true));
    };

    directory.createDirectory();

    saveToFileButton.onClick = [&]
    {
        auto fileName = deviceSettings->getValue("device_name").toString();
        if (fileName.isEmpty())
        {
            fileName = "Unknown Device";
        }
        if (const auto serialNumber = deviceSettings->getValue("serial_number").toString(); serialNumber.isNotEmpty())
        {
            fileName += " " + serialNumber;
        }

        fileChooser = std::make_unique<juce::FileChooser>("Save Device Settings", directory.getChildFile(fileName), "*.json");
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [&](const auto&)
        {
            if (fileChooser->getResult() == juce::File())
            {
                return;
            }

            juce::DynamicObject object;
            for (const auto& command : deviceSettings->getWriteCommands(false))
            {
                object.setProperty(juce::String(command.key), command.getValue());
            }

            juce::FileOutputStream stream(fileChooser->getResult());
            stream.setPosition(0);
            stream.truncate();
            object.writeAsJSON(stream, {});
        });
    };

    loadFromFileButton.onClick = [&]
    {
        std::optional<juce::File> defaultFile;
        for (auto file : directory.findChildFiles(juce::File::findFiles, false, "*.json"))
        {
            if (file.getFileName().contains(deviceSettings->getValue("serial_number").toString()) && (file.getLastModificationTime() > defaultFile.value_or(juce::File()).getLastModificationTime()))
            {
                defaultFile = file;
            }
        }

        fileChooser = std::make_unique<juce::FileChooser>("Load Device Settings", defaultFile.value_or(directory), "*.json");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [&](const auto&)
        {
            if (fileChooser->getResult() == juce::File())
            {
                return;
            }

            const auto parsed = juce::JSON::parse(fileChooser->getResult().loadFileAsString());
            if (auto* const object = parsed.getDynamicObject())
            {
                for (const auto& command : object->getProperties())
                {
                    deviceSettings->setValue({ command.name.toString(), command.value });
                }
            }

            if (readFromValueTree().writeSettingsWhenModified)
            {
                writeCommands(deviceSettings->getWriteCommands(true));
            }
        });
    };

    defaultsButton.onClick = [this]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<AreYouSureDialog>("Are you sure you want to restore default device settings?"), [this]
        {
            enableInProgress(deviceSettings->getReadCommands());

            connectionPanel.sendCommands({ { "default", {} } }, this, [this](const auto& responses)
            {
                if (responses.empty())
                {
                    disableInProgress();
                    DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Unable to confirm default command."));
                    return;
                }

                connectionPanel.sendCommands({ { "save", {} } }, this, [this](const auto& responses_)
                {
                    if (responses_.empty())
                    {
                        disableInProgress();
                        DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Unable to confirm save command."));
                        return;
                    }

                    connectionPanel.sendCommands({ { "apply", {} } }, this, [this](const auto&)
                    {
                        readAllButton.onClick();
                    });
                });
            });
            return true;
        });
    };

    valueTreePropertyChanged(settingsTree, "customSchema");
}

void DeviceSettingsWindow::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::backgroundLight);

    g.setColour(UIColours::backgroundDark);
    g.fillRect(buttonBounds);

    Window::paint(g);
}

void DeviceSettingsWindow::resized()
{
    Window::resized();

    auto bounds = getContentBounds();
    disabledOverlay.setBounds(bounds);

    buttonBounds = bounds.removeFromBottom(25).toFloat();
    const auto buttonWidth = buttonBounds.getWidth() / buttons.size();

    deviceSettings->setBounds(bounds);
    juce::ScopedValueSetter _(buttonBounds, buttonBounds);
    for (auto* const button : buttons)
    {
        button->setBounds(buttonBounds.removeFromLeft(buttonWidth).reduced(2.0f).toNearestInt());
    }
}

void DeviceSettingsWindow::expandOrCollapseAll(juce::TreeViewItem& rootItem, const bool expand)
{
    rootItem.setOpen(expand || rootItem.getParentItem() == nullptr);
    for (int index = 0; index < rootItem.getNumSubItems(); index++)
    {
        expandOrCollapseAll(*rootItem.getSubItem(index), expand);
    }
}

void DeviceSettingsWindow::writeToValueTree(const Settings& settings)
{
    settingsTree.setProperty("hideUnusedSettings", settings.hideUnusedSettings, nullptr);
    settingsTree.setProperty("readSettingsWhenWindowOpens", settings.readSettingsWhenWindowOpens, nullptr);
    settingsTree.setProperty("writeSettingsWhenModified", settings.writeSettingsWhenModified, nullptr);
    if (settingsTree.getProperty("customSchema") != settings.customSchema.getFullPathName()) // avoid callback when property doesn't exist
    {
        settingsTree.setProperty("customSchema", settings.customSchema.getFullPathName(), nullptr);
    }
}

DeviceSettingsWindow::Settings DeviceSettingsWindow::readFromValueTree()
{
    Settings settings;
    settings.hideUnusedSettings = settingsTree.getProperty("hideUnusedSettings", settings.hideUnusedSettings);
    settings.readSettingsWhenWindowOpens = settingsTree.getProperty("readSettingsWhenWindowOpens", settings.readSettingsWhenWindowOpens);
    settings.writeSettingsWhenModified = settingsTree.getProperty("writeSettingsWhenModified", settings.writeSettingsWhenModified);
    settings.customSchema = settingsTree.getProperty("customSchema");
    return settings;
}

void DeviceSettingsWindow::writeCommands(const std::vector<CommandMessage>& commands)
{
    enableInProgress(commands);

    connectionPanel.sendCommands(commands, this, [&, commands](const auto& responses)
    {
        for (const auto& command : commands)
        {
            const auto response = std::find(responses.begin(), responses.end(), command);

            if (response == responses.end() || response->error)
            {
                deviceSettings->setStatus(command.key, Setting::Status::warning, "Unable to Write to Device");
                continue;
            }

            deviceSettings->setValue(*response);
            deviceSettings->setStatus(response->key, Setting::Status::normal, {});
        }

        writeAllButton.setToggleState(commands.size() != responses.size(), juce::dontSendNotification);

        connectionPanel.sendCommands({ { "save", {} } }, this, [&](const auto& responses_)
        {
            disableInProgress();

            if (responses_.empty())
            {
                DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Unable to confirm save command."));
                return;
            }

            connectionPanel.sendCommands({ { "apply", {} } });
        });
    });
}

void DeviceSettingsWindow::enableInProgress(const std::vector<CommandMessage>& commands)
{
    for (const auto& command : commands)
    {
        deviceSettings->setStatus(command.key, Setting::Status::normal, {});
    }
    readAllButton.setToggleState(false, juce::dontSendNotification);
    writeAllButton.setToggleState(false, juce::dontSendNotification);

    disabledOverlay.setVisible(true);
}

void DeviceSettingsWindow::disableInProgress()
{
    juce::Timer::callAfterDelay(100, [&, self = SafePointer<juce::Component>(this)]
    {
        if (self != nullptr)
        {
            disabledOverlay.setVisible(false);
        }
    });
}

juce::PopupMenu DeviceSettingsWindow::getMenu()
{
    juce::PopupMenu menu = Window::getMenu();

    menu.addItem("Read Settings When Window Opens", true, readFromValueTree().readSettingsWhenWindowOpens, [&]
    {
        auto settings = readFromValueTree();
        settings.readSettingsWhenWindowOpens = !settings.readSettingsWhenWindowOpens;
        writeToValueTree(settings);
    });
    menu.addItem("Write Settings When Modified", true, readFromValueTree().writeSettingsWhenModified, [&]
    {
        auto settings = readFromValueTree();
        settings.writeSettingsWhenModified = !settings.writeSettingsWhenModified;
        writeToValueTree(settings);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("VIEW"), nullptr);
    menu.addItem("Hide Unused Settings", true, readFromValueTree().hideUnusedSettings, [&]
    {
        auto settings = readFromValueTree();
        settings.hideUnusedSettings = !settings.hideUnusedSettings;
        writeToValueTree(settings);
    });
    menu.addItem("Expand All", [&]
    {
        expandOrCollapseAll(*deviceSettings->getRootItem(), true);
    });
    menu.addItem("Collapse All", [&]
    {
        expandOrCollapseAll(*deviceSettings->getRootItem(), false);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("SCHEMA"), nullptr);
    menu.addItem("x-IMU3", true, readFromValueTree().customSchema == juce::File(), [&]
    {
        auto settings = readFromValueTree();
        settings.customSchema = juce::File();
        writeToValueTree(settings);
    });
    juce::PopupMenu customSchemasMenu;
    customSchemasMenu.addItem("Load .xml File", [&]
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select Schema", juce::File(), "*.xml");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [&](const auto&)
        {
            if (fileChooser->getResult() == juce::File())
            {
                return;
            }

            const auto customSchema = customSchemasDirectory.getChildFile(fileChooser->getResult().getFileName());
            customSchemasDirectory.createDirectory();
            fileChooser->getResult().copyFileTo(customSchema);

            if (readFromValueTree().customSchema == customSchema)
            {
                settingsTree.sendPropertyChangeMessage("customSchema");
            }
            else
            {
                auto settings = readFromValueTree();
                settings.customSchema = customSchema;
                writeToValueTree(settings);
            }
        });
    });
    if (const auto files = customSchemasDirectory.findChildFiles(juce::File::findFiles, false, "*.xml"); files.isEmpty() == false)
    {
        customSchemasMenu.addSeparator();
        customSchemasMenu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("PREVIOUS"), nullptr);
        for (const auto& file : files)
        {
            const auto ticked = readFromValueTree().customSchema == file.getFullPathName();
            customSchemasMenu.addItem(file.getFileNameWithoutExtension(), true, ticked, [&, file]
            {
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

void DeviceSettingsWindow::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if (treeWhosePropertyHasChanged != settingsTree)
    {
        return;
    }

    if (property.toString() == "customSchema")
    {
        const auto tree = [&]
        {
            if (const auto customSchemaFile = readFromValueTree().customSchema; customSchemaFile != juce::File())
            {
                return juce::ValueTree::fromXml(juce::File(customSchemaFile).loadFileAsString());
            }
            return juce::ValueTree::fromXml(BinaryData::DeviceSettings_xml);
        }();

        deviceSettings = std::make_unique<DeviceSettings>(tree, [this](const CommandMessage& command)
        {
            if (readFromValueTree().writeSettingsWhenModified)
            {
                writeCommands({ command });
            }
        });
        addAndMakeVisible(*deviceSettings);
        resized();

        if (readFromValueTree().readSettingsWhenWindowOpens)
        {
            readAllButton.onClick();
        }
    }

    if (deviceSettings)
    {
        deviceSettings->setHideUnusedSettings(readFromValueTree().hideUnusedSettings);
    }
}
