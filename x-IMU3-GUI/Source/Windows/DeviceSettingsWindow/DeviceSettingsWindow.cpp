#include "ConnectionPanel/ConnectionPanel.h"
#include "DeviceSettingsWindow.h"
#include "Dialogs/MessageDialog.h"
#include "Widgets/PopupMenuHeader.h"

const std::vector<std::pair<juce::String, const char *> > DeviceSettingsWindow::defaultSchemas
{
    {"x-IMU3", BinaryData::xIMU3_Schema_xml},
    {"x-IMU4", BinaryData::xIMU4_Schema_xml},
};

DeviceSettingsWindow::DeviceSettingsWindow(const juce::ValueTree &windowLayout_, const juce::Identifier &type_, ConnectionPanel &connectionPanel_, juce::ThreadPool &threadPool_)
    : Window(windowLayout_, type_, connectionPanel_, "Device Settings Menu"),
      threadPool(threadPool_) {
    addAndMakeVisible(syncButton);
    addAndMakeVisible(backupButton);
    addAndMakeVisible(restoreButton);
    addAndMakeVisible(defaultsButton);
    addChildComponent(disabledOverlay);

    syncButton.onClick = [&] {
        if (getSchema() == "Enumerate") {
            handleAsyncUpdate();
            return;
        }

        syncSettings();
    };

    backupButton.onClick = [this] {
        juce::String fileName = "Unknown Device";
        if (auto response = connectionPanel.getConnection()->getPingResponse()) {
            fileName = juce::String(response->device_name) + " " + juce::String(response->serial_number);
        }

        std::ignore = deviceSettingsDirectory.createDirectory();
        fileChooser = std::make_unique<juce::FileChooser>("Backup Settings", deviceSettingsDirectory.getChildFile(fileName), "*.json");
        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting, [this](const auto &) {
            if (fileChooser->getResult() == juce::File()) {
                return;
            }

            const auto file = fileChooser->getResult().withFileExtension("json");
            std::ignore = file.deleteFile();

            disabledOverlay.setVisible(true);

            threadPool.addJob([this, self = SafePointer<juce::Component>(this), connection = connectionPanel.getConnection(), filePath = file.getFullPathName().toStdString()] {
                const auto result = ximu3::settings_backup(filePath, *connection);

                juce::MessageManager::callAsync([this, self, result] {
                    if (self == nullptr) {
                        return;
                    }

                    disabledOverlay.setVisible(false);

                    if (result != ximu3::XIMU3_ResultOk) {
                        DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Backup failed: " + juce::String(ximu3::XIMU3_result_to_string(result))));
                    }
                });
            });
        });
    };

    restoreButton.onClick = [this] {
        std::ignore = deviceSettingsDirectory.createDirectory();

        juce::File mostRecent;
        if (auto response = connectionPanel.getConnection()->getPingResponse()) {
            for (auto file: deviceSettingsDirectory.findChildFiles(juce::File::findFiles, false, "*" + juce::String(response->serial_number) + "*.*json")) {
                if (file.getLastModificationTime() > mostRecent.getLastModificationTime()) {
                    mostRecent = file;
                }
            }
        }

        fileChooser = std::make_unique<juce::FileChooser>("Select Backup", mostRecent.existsAsFile() ? mostRecent : deviceSettingsDirectory, "*.json");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this](const auto &) {
            if (fileChooser->getResult() == juce::File()) {
                return;
            }

            disabledOverlay.setVisible(true);

            threadPool.addJob([this, self = SafePointer<juce::Component>(this), connection = connectionPanel.getConnection(), filePath = fileChooser->getResult().getFullPathName().toStdString()] {
                const auto result = ximu3::settings_restore(filePath, *connection);

                juce::MessageManager::callAsync([this, self, result] {
                    if (self == nullptr) {
                        return;
                    }

                    if (result != ximu3::XIMU3_ResultOk) {
                        disabledOverlay.setVisible(false);
                        DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Restore failed: " + juce::String(ximu3::XIMU3_result_to_string(result))));
                        return;
                    }

                    sendCommand("save", false, [this] {
                        sendCommand("apply", true, [this] {
                            disabledOverlay.setVisible(false);
                            syncButton.onClick();
                        });
                    });
                });
            });
        });
    };

    defaultsButton.onClick = [this] {
        DialogQueue::getSingleton().pushFront(std::make_unique<ConfirmDialog>("Are you sure you want to restore default settings?"), [this] {
            disabledOverlay.setVisible(true);

            sendCommand("default", false, [this] {
                sendCommand("save", false, [this] {
                    sendCommand("apply", true, [this] {
                        disabledOverlay.setVisible(false);
                        syncButton.onClick();
                    });
                });
            });
            return true;
        });
    };

    handleAsyncUpdate();

    setOpaque(true);
}

void DeviceSettingsWindow::paint(juce::Graphics &g) {
    g.fillAll(UIColours::backgroundLight);

    g.setColour(UIColours::backgroundDark);
    g.fillRect(juce::Rectangle{0, syncButton.getY(), getWidth(), syncButton.getHeight()});

    enumerationError.draw(g, getContentBounds().toFloat());
}

void DeviceSettingsWindow::resized() {
    Window::resized();

    auto bounds = getContentBounds();
    disabledOverlay.setBounds(bounds);

    auto footer = bounds.removeFromBottom(25);
    const auto buttonWidth = footer.getWidth() / 4;
    syncButton.setBounds(footer.removeFromLeft(buttonWidth).reduced(2));
    backupButton.setBounds(footer.removeFromLeft(buttonWidth).reduced(2));
    restoreButton.setBounds(footer.removeFromLeft(buttonWidth).reduced(2));
    defaultsButton.setBounds(footer.reduced(2));

    if (treeView != nullptr) {
        treeView->setBounds(bounds);
    }
}

bool DeviceSettingsWindow::syncWhenWindowOpens() const {
    return settingsTree.getProperty("syncWhenWindowOpens", true);
}

bool DeviceSettingsWindow::hideUnusedSettings() const {
    return settingsTree.getProperty("hideUnusedSettings", true);
}

juce::String DeviceSettingsWindow::getSchema() const {
    const juce::String schema = settingsTree["schema"];

    if (schema == "Enumerate") {
        return schema;
    }

    for (auto schema_: defaultSchemas) {
        if (schema == schema_.first) {
            return schema;
        }
    }

    if (juce::File::isAbsolutePath(schema) && juce::File(schema).existsAsFile()) {
        return schema;
    }

    return defaultSchemas.front().first;
}

void DeviceSettingsWindow::syncSettings() {
    if (treeView == nullptr) {
        return;
    }

    disabledOverlay.setVisible(true);

    std::vector<std::string> commands;
    for (auto *const setting: treeView->getSettings()) {
        setting->clear();
        commands.push_back(setting->getReadCommand());
    }

    treeView->refresh();

    connectionPanel.sendCommands(commands, this, [&](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
        disabledOverlay.setVisible(false);

        for (size_t index = 0; index < responses.size(); index++) {
            treeView->getSettings()[index]->receive(responses[index]);
        }

        treeView->refresh();

        connectionPanel.getConnection()->pingAsync([](auto) {
        });
    });
}

void DeviceSettingsWindow::loadSchema(std::unique_ptr<Schema::Group> group) {
    if (group == nullptr) {
        treeView.reset();
        return;
    }

    const auto write = [this](Schema::Setting &setting, const std::string &command) {
        treeView->refresh();

        connectionPanel.sendCommands({command}, this, [this, &setting](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
            setting.receive(responses.front());

            treeView->refresh();

            connectionPanel.getConnection()->pingAsync([](auto) {
            });
        });
    };

    treeView = std::make_unique<TreeView>(std::move(group), write, [this] { return hideUnusedSettings(); });
    addAndMakeVisible(*treeView);
    resized();

    if (syncWhenWindowOpens()) {
        syncSettings();
    }
}

void DeviceSettingsWindow::sendCommand(const std::string &key, const bool silent, std::function<void()> callback) {
    const std::string command = "{\"" + key + "\":null}";

    connectionPanel.sendCommands({command}, this, [command, silent, callback](const auto &responses) {
        if (silent == false) {
            if (responses.front().has_value() == false) {
                DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("No response to " + command));
                return;
            }

            if (responses.front()->error.has_value()) {
                DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Error response to " + command + ": " + *responses.front()->error));
            }
        }

        callback();
    });
}

juce::PopupMenu DeviceSettingsWindow::getMenu() {
    juce::PopupMenu menu = Window::getMenu();

    menu.addItem("Sync When Window Opens", true, syncWhenWindowOpens(), [&] {
        settingsTree.setProperty("syncWhenWindowOpens", !syncWhenWindowOpens(), nullptr);
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("VIEW"), nullptr);
    menu.addItem("Hide Unused Settings", true, hideUnusedSettings(), [&] {
        settingsTree.setProperty("hideUnusedSettings", !hideUnusedSettings(), nullptr);
    });
    menu.addItem("Expand All", [&] {
        if (treeView != nullptr) {
            treeView->expandOrCollapseAll(true);
        }
    });
    menu.addItem("Collapse All", [&] {
        if (treeView != nullptr) {
            treeView->expandOrCollapseAll(false);
        }
    });

    menu.addSeparator();
    menu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("SCHEMA"), nullptr);
    menu.addItem("Enumerate", true, getSchema() == "Enumerate", [&] {
        settingsTree.setProperty("schema", "Enumerate", nullptr);
    });
    for (auto schema: defaultSchemas) {
        menu.addItem(schema.first, true, getSchema() == schema.first, [&, schema] {
            settingsTree.setProperty("schema", schema.first, nullptr);
        });
    }

    juce::PopupMenu customSchemasMenu;
    customSchemasMenu.addItem("Load Schema", [&] {
        fileChooser = std::make_unique<juce::FileChooser>("Select Schema", juce::File(), "*.xml");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [&](const auto &) {
            if (fileChooser->getResult() == juce::File()) {
                return;
            }

            const auto customSchema = schemasDirectory.getChildFile(fileChooser->getResult().getFileName());
            std::ignore = schemasDirectory.createDirectory();
            std::ignore = fileChooser->getResult().copyFileTo(customSchema);
            settingsTree.setProperty("schema", customSchema.getFullPathName(), nullptr);
        });
    });
    if (const auto files = schemasDirectory.findChildFiles(juce::File::findFiles, false, "*.xml"); files.isEmpty() == false) {
        customSchemasMenu.addSeparator();
        customSchemasMenu.addCustomItem(-1, std::make_unique<PopupMenuHeader>("PREVIOUS"), nullptr);
        for (const auto &file: files) {
            customSchemasMenu.addItem(file.getFileName(), true, file.getFullPathName() == getSchema(), [&, file] {
                settingsTree.setProperty("schema", file.getFullPathName(), nullptr);
            });
        }
    }

    const auto ticked = juce::File::isAbsolutePath(getSchema());
    const auto suffix = ticked ? (" (" + juce::File(getSchema()).getFileName() + ")") : "";
    menu.addSubMenu("Custom" + suffix, customSchemasMenu, true, nullptr, ticked);

    return menu;
}

void DeviceSettingsWindow::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) {
    if (treeWhosePropertyHasChanged != settingsTree) {
        return;
    }

    if (property.toString() == "schema") {
        triggerAsyncUpdate();
        return;
    }

    if (property.toString() == "hideUnusedSettings") {
        if (treeView != nullptr) {
            treeView->refresh();
        }
    }
}

void DeviceSettingsWindow::handleAsyncUpdate() {
    enumerationError = {};
    repaint();

    if (getSchema() == "Enumerate") {
        disabledOverlay.setVisible(true);

        threadPool.addJob([this, self = SafePointer<juce::Component>(this)] {
            try {
                juce::MessageManager::callAsync([this, self, schema = Schema::loadSchema(connectionPanel.getConnection())]() mutable {
                    if (self == nullptr) {
                        return;
                    }

                    disabledOverlay.setVisible(false);
                    loadSchema(std::move(schema));
                });
            } catch (const std::exception &e) {
                juce::MessageManager::callAsync([this, self, error = e.what()] {
                    if (self == nullptr) {
                        return;
                    }

                    enumerationError.setJustification(juce::Justification::centred);
                    enumerationError.append("Enumeration Failed\n", UIColours::foreground);
                    enumerationError.append(error, juce::Colours::grey);
                    enumerationError.setFont(UIFonts::getDefaultFont());
                    repaint();

                    disabledOverlay.setVisible(false);
                    loadSchema({});
                });
            }
        });
        return;
    }

    for (auto schema: defaultSchemas) {
        if (getSchema() == schema.first) {
            loadSchema(Schema::loadSchema(juce::ValueTree::fromXml(schema.second)));
            return;
        }
    }

    loadSchema(Schema::loadSchema(juce::ValueTree::fromXml(juce::File(getSchema()).loadFileAsString())));
}
