#include "DeviceSettingsXWindow.h"
#include "ConnectionPanel/ConnectionPanel.h"

DeviceSettingsXWindow::DeviceSettingsXWindow(const juce::ValueTree &windowLayout_, const juce::Identifier &type_, ConnectionPanel &connectionPanel_)
    : Window(windowLayout_, type_, connectionPanel_, "Device Settings Menu") {
    addAndMakeVisible(readAllButton);
    addAndMakeVisible(writeAllButton);

    readAllButton.onClick = [&] {
        std::vector<std::string> commands;
        for (const auto &setting: tree->settings) {
            commands.push_back(setting->readCommand());
        }

        connectionPanel.sendCommands(commands, this, [&, commands](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
            for (size_t index = 0; index < responses.size(); index++) {
                tree->settings[index]->receiveResponse(responses[index]);
                tree->settingItems[index]->refresh();
            }
        });
    };

    writeAllButton.onClick = [&] {
        std::vector<std::string> commands;
        for (size_t index = 0; index < tree->settings.size(); index++) {
            commands.push_back(tree->settings[index]->writeCommand());
        }

        connectionPanel.sendCommands(commands, this, [&, commands](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
            for (size_t index = 0; index < responses.size(); index++) {
                tree->settings[index]->receiveResponse(responses[index]);
                tree->settingItems[index]->refresh();
            }
        });
    };

    setGroup({
        "Root",
        {
            Group{
                "Device Information", {
                    SettingX{"Device Name", "device_name", SettingX::Type::string},
                    SettingX{"Serial Number", "serial_number", SettingX::Type::string, {}, true},
                }
            },
            Group{
                "Serial", {
                    SettingX{"RTS/CTS", "serial_rts_cts_enabled", SettingX::Type::boolean},

                    Group{
                        "Accessory Send Conditions", {
                            SettingX{"Number of Bytes", "serial_accessory_number_of_bytes", SettingX::Type::number},
                        }
                    },
                }
            },
        }
    });

    setOpaque(true);
}

void DeviceSettingsXWindow::paint(juce::Graphics &g) {
    g.setColour(UIColours::backgroundDark);
}

void DeviceSettingsXWindow::resized() {
    Window::resized();

    auto bounds = getContentBounds();

    auto footer = bounds.removeFromBottom(25);
    readAllButton.setBounds(footer.removeFromLeft(footer.getWidth() / 2));
    writeAllButton.setBounds(footer);

    tree->treeView.setBounds(bounds);
}

void DeviceSettingsXWindow::setGroup(Group group) {
    tree = std::make_unique<Tree>(group);
    addAndMakeVisible(tree->treeView);
    resized();
}

juce::PopupMenu DeviceSettingsXWindow::getMenu() {
    juce::PopupMenu menu = Window::getMenu();

    menu.addItem("Enumerate", [&] {
        juce::Thread::launch([&] {
            std::vector<SettingX> newSettings;

            while (true) {
                const auto response = connectionPanel.getConnection()->sendCommand({"{\"enumerate " + std::to_string(newSettings.size()) + "\": null}"});

                if (response.has_value() == false) {
                    return; // TODO: handle error
                }

                if (response->error.has_value()) {
                    return; // TODO: handle error
                }

                if (response->value == "null") {
                    break;
                }

                const auto value = ximu3::CommandMessage::parse(response->value);

                if (value.has_value() == false) {
                    return; // TODO: handle error
                }

                // TODO: What if type is array?
                auto type = SettingX::checkType(value->value);

                if (type.has_value() == false) {
                    return; // TODO: handle error
                }

                newSettings.push_back({value->key, value->key, *type, {}, false});
            }

            Group group;
            for (auto &setting: newSettings) {
                group.items.push_back(setting);
            }

            juce::MessageManager::callAsync([&, group] {
                setGroup(group);
            });
        });
    });

    return menu;
}
