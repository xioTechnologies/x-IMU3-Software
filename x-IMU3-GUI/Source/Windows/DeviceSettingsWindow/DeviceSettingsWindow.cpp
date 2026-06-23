#include "ConnectionPanel/ConnectionPanel.h"
#include "DeviceSettingsWindow.h"
#include "Dialogs/MessageDialog.h"
#include "Schema/Schema.h"
#include "Widgets/PopupMenuHeader.h"

DeviceSettingsWindow::DeviceSettingsWindow(const juce::ValueTree &windowLayout_, const juce::Identifier &type_, ConnectionPanel &connectionPanel_, juce::ThreadPool &threadPool_)
    : Window(windowLayout_, type_, connectionPanel_, "Device Settings Menu"),
      threadPool(threadPool_) {
    addAndMakeVisible(syncButton);
    addAndMakeVisible(backupButton);
    addAndMakeVisible(restoreButton);
    addAndMakeVisible(defaultsButton);
    addChildComponent(disabledOverlay);

    syncButton.onClick = [&] {
        if (currentSchema == "enumerate") {
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

                    sendCommandSequence(
                        {
                            {.key = "save", .handleError = true},
                            {.key = "apply", .handleError = false},
                        },
                        [this] {
                            disabledOverlay.setVisible(false);
                            syncButton.onClick();
                        });
                });
            });
        });
    };

    defaultsButton.onClick = [this] {
        DialogQueue::getSingleton().pushFront(std::make_unique<ConfirmDialog>("Are you sure you want to restore default settings?"), [this] {
            disabledOverlay.setVisible(true);

            sendCommandSequence(
                {
                    {.key = "default", .handleError = true},
                    {.key = "save", .handleError = true},
                    {.key = "apply", .handleError = false},
                },
                [this] {
                    disabledOverlay.setVisible(false);
                    syncButton.onClick();
                });
            return true;
        });
    };

    pingCallbackId = connectionPanel.getConnection()->addPingCallback(pingCallback = [&, model = std::optional<juce::String>()](auto response) mutable {
        if (model == juce::String(response.device_name)) {
            return;
        }

        model = response.device_name;

        juce::MessageManager::callAsync([&, self = SafePointer<juce::Component>(this)] {
            if (self == nullptr) {
                return;
            }

            if (currentSchema == "auto") {
                triggerAsyncUpdate();
            }
        });
    });

    handleAsyncUpdate();

    setOpaque(true);
}

DeviceSettingsWindow::~DeviceSettingsWindow() {
    connectionPanel.getConnection()->removeCallback(pingCallbackId);
}

void DeviceSettingsWindow::paint(juce::Graphics &g) {
    g.fillAll(UIColours::backgroundLight);

    g.setColour(UIColours::backgroundDark);
    g.fillRect(juce::Rectangle{0, syncButton.getY(), getWidth(), syncButton.getHeight()});

    info.draw(g, getContentBounds().toFloat());
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

void DeviceSettingsWindow::setSchema(const juce::String &schema) {
    settingsTree.setProperty("schema", schema, nullptr);
    settingsTree.sendPropertyChangeMessage("schema");
}

void DeviceSettingsWindow::showInfo(const juce::String &heading, const juce::String &message) {
    info = {};
    info.setJustification(juce::Justification::centred);
    info.append(heading, UIColours::foreground);
    if (message.isNotEmpty()) {
        info.append("\n" + message, juce::Colours::grey);
    }
    info.setFont(UIFonts::getDefaultFont());
    repaint();
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

    connectionPanel.sendCommands(commands, treeView.get(), [&](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
        disabledOverlay.setVisible(false);

        for (size_t index = 0; index < responses.size(); index++) {
            treeView->getSettings()[index]->receive(responses[index]);
        }

        treeView->refresh();

        connectionPanel.getConnection()->pingAsync([](auto) {
        });
    });
}

void DeviceSettingsWindow::loadSchema(const Schema &schema) {
    loadSchema(SettingsSchema::loadSchema(schema.settings));
}

void DeviceSettingsWindow::loadSchema(std::unique_ptr<SettingsSchema::Group> group) {
    if (group == nullptr) {
        treeView.reset();
        return;
    }

    const auto write = [this](SettingsSchema::Setting &setting, const std::string &command) {
        treeView->refresh();

        disabledOverlay.setVisible(true);

        connectionPanel.sendCommands({command}, treeView.get(), [this, &setting](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
            setting.receive(responses.front());

            treeView->refresh();

            if (setting.status != SettingsSchema::Setting::Status::confirmed) {
                disabledOverlay.setVisible(false);
                return;
            }

            sendCommandSequence(
                {
                    {.key = "ping", .handleError = false},
                    {.key = "save", .handleError = true},
                    {.key = "apply", .handleError = false},
                },
                [this] {
                    disabledOverlay.setVisible(false);
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

void DeviceSettingsWindow::sendCommandSequence(std::vector<CommandSequenceItem> sequence, std::function<void()> epilogue) {
    if (sequence.empty()) {
        epilogue();
        return;
    }

    const auto command = "{\"" + sequence.front().key + "\":null}";

    connectionPanel.sendCommands({command}, this, [this, sequence, epilogue, command](const auto &responses) mutable {
        if (sequence.front().handleError) {
            if (responses.front().has_value() == false) {
                DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("No response to " + command));
                epilogue();
                return;
            }

            if (responses.front()->error.has_value()) {
                DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Error response to " + command + ": " + *responses.front()->error));
                epilogue();
                return;
            }
        }

        sequence.erase(sequence.begin());
        sendCommandSequence(sequence, epilogue);
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
    juce::String autoText = "Auto";
    if (currentSchema == "auto" && autoSchemaName.isNotEmpty()) {
        autoText += " (" + autoSchemaName + ")";
    }
    menu.addItem(autoText, true, currentSchema == "auto", [&] {
        setSchema("auto");
    });
    menu.addItem("Enumerate", true, currentSchema == "enumerate", [&] {
        setSchema("enumerate");
    });
    for (const auto &schema: Schema::getAll()) {
        menu.addItem(schema.name, true, currentSchema == schema.fileName, [&, fileName = schema.fileName] {
            setSchema(fileName);
        });
    }
    menu.addItem("Add Schema...", [&] {
        fileChooser = Schema::add([&](const auto &file) {
            setSchema(file.getFileName());
        });
    });

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
    showInfo({});
    loadSchema(nullptr);
    disabledOverlay.setVisible(false);

    if (const auto schema = Schema::get(settingsTree["schema"])) {
        currentSchema = schema->fileName;

        loadSchema(*schema);
        return;
    }

    if (settingsTree["schema"] == "enumerate") {
        currentSchema = "enumerate";

        showInfo("Enumerating...");
        disabledOverlay.setVisible(true);

        threadPool.addJob([this, self = SafePointer<juce::Component>(this), connection = connectionPanel.getConnection()] {
            try {
                juce::MessageManager::callAsync([this, self, schema = SettingsSchema::loadSchema(connection)]() mutable {
                    if (self == nullptr) {
                        return;
                    }

                    showInfo("");
                    disabledOverlay.setVisible(false);
                    loadSchema(std::move(schema));
                });
            } catch (const std::exception &e) {
                juce::MessageManager::callAsync([this, self, error_ = juce::String(e.what())] {
                    if (self == nullptr) {
                        return;
                    }

                    showInfo("Enumeration Failed", error_);
                    disabledOverlay.setVisible(false);
                });
            }
        });
        return;
    }

    currentSchema = "auto";
    autoSchemaName = "";

    const auto response = connectionPanel.getConnection()->getPingResponse();
    if (response.has_value() == false) {
        showInfo("Auto Schema", "Waiting for ping response");
        return;
    }

    const juce::String model = response->device_name; // TODO: Use model

    const auto schema = Schema::find(model);
    if (schema.has_value() == false) {
        showInfo("Auto Schema", "Schema not found for " + model);
        return;
    }

    autoSchemaName = schema->name;
    loadSchema(*schema);
}
