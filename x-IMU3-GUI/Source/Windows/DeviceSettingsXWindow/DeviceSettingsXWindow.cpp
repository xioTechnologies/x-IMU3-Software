#include "DeviceSettingsXWindow.h"
#include "ConnectionPanel/ConnectionPanel.h"

DeviceSettingsXWindow::DeviceSettingsXWindow(const juce::ValueTree &windowLayout_, const juce::Identifier &type_, ConnectionPanel &connectionPanel_)
    : Window(windowLayout_, type_, connectionPanel_, "Device Settings Menu") {
    setSettings({
        {"Device Name", "device_name", DeviceSettingX::Type::string},
        {"Serial Number", "serial_number", DeviceSettingX::Type::string, {}, true},
        {"Max File Size", "data_logger_max_file_size", DeviceSettingX::Type::number},
        {
            "AHRS Message Type", "ahrs_message_type", DeviceSettingX::Type::number,
            {
                {0, "a"},
                {1, "b"},
                {2, "c"},
                {3, "d"},
                {4, "e"},
            },
        },
        {"Binary Mode", "binary_mode_enabled", DeviceSettingX::Type::boolean},
        {
            "SendMessageRateDivisorMagnetometer", "magnetometer_message_rate_divisor", DeviceSettingX::Type::number, {
                {0, "Disabled"},
                {20, "1 Hz"},
                {10, "2 Hz"},
                {5, "4 Hz"},
                {4, "5 Hz"},
                {2, "10 Hz"},
                {1, "20 Hz"},
            },
        },
    });

    addAndMakeVisible(readAllButton);
    addAndMakeVisible(writeAllButton);

    readAllButton.onClick = [&] {
        std::vector<std::string> commands;
        for (const auto &setting: settings) {
            commands.push_back(setting.readCommand());
        }

        connectionPanel.sendCommands(commands, this, [&, commands](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
            for (size_t index = 0; index < responses.size(); index++) {
                settings[index].receiveResponse(responses[index]);
                components[index]->refresh();
            }
        });
    };

    writeAllButton.onClick = [&] {
        std::vector<std::string> commands;
        for (size_t index = 0; index < settings.size(); index++) {
            if (const auto value = components[index]->getValue()) {
                commands.push_back(settings[index].writeCommand(*value));
            }
        }

        connectionPanel.sendCommands(commands, this, [&, commands](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
            for (size_t index = 0; index < responses.size(); index++) {
                settings[index].receiveResponse(responses[index]);
                components[index]->refresh();
            }
        });
    };

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

    for (auto &component: components) {
        bounds.removeFromTop(5);
        component->setBounds(bounds.removeFromTop(25));
    }
}

void DeviceSettingsXWindow::setSettings(const std::vector<DeviceSettingX> &settings_) {
    components.clear();

    settings = settings_;

    for (auto &setting: settings) {
        auto component = std::make_unique<SettingComponent>(setting);
        addAndMakeVisible(*component);
        components.push_back(std::move(component));
    }

    resized();
}

juce::PopupMenu DeviceSettingsXWindow::getMenu() {
    juce::PopupMenu menu = Window::getMenu();

    menu.addItem("Enumerate", [&] {
        juce::Thread::launch([&] {
            std::vector<DeviceSettingX> newSettings;

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
                auto type = DeviceSettingX::checkType(value->value);

                if (type.has_value() == false) {
                    return; // TODO: handle error
                }

                newSettings.push_back({value->key, value->key, *type, {}, false});
            }

            juce::MessageManager::callAsync([&, newSettings] {
                setSettings(newSettings);
            });
        });
    });

    return menu;
}
