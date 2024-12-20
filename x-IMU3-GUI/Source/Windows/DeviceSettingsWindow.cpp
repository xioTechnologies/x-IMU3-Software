#include "ConnectionPanel/ConnectionPanel.h"
#include "DeviceSettingsWindow.h"
#include "Dialogs/MessageDialog.h"
#include "Firmware/Firmware.h"

DeviceSettingsWindow::DeviceSettingsWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_)
    : Window(windowLayout_, type_, connectionPanel_, "Device Settings Menu")
{
    addAndMakeVisible(deviceSettings);
    addAndMakeVisible(readAllButton);
    addAndMakeVisible(writeAllButton);
    addAndMakeVisible(saveToFileButton);
    addAndMakeVisible(loadFromFileButton);
    addAndMakeVisible(defaultsButton);
    addChildComponent(disabledOverlay);

    readAllButton.onClick = [this]
    {
        const auto commands = deviceSettings.getReadCommands();

        enableInProgress(commands);

        connectionPanel.sendCommands(commands, this, [&, commands](const std::vector<CommandMessage>& responses)
        {
            for (const auto& command : commands)
            {
                const auto response = std::find(responses.begin(), responses.end(), command);

                if (response == responses.end() || response->error)
                {
                    deviceSettings.setStatus(command.key, Setting::Status::warning, "Unable to Read from Device");
                    continue;
                }

                deviceSettings.setValue(*response);

                if (CommandMessage::normaliseKey(command.key) == CommandMessage::normaliseKey("firmware_version") && response->getValue() != Firmware::version)
                {
                    deviceSettings.setStatus(command.key, Setting::Status::warning, "Unexpected Firmware Version");
                    continue;
                }

                deviceSettings.setStatus(response->key, Setting::Status::normal, {});
            }

            readAllButton.setToggleState(commands.size() != responses.size(), juce::dontSendNotification);

            disableInProgress();
        });
    };

    if (ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens)
    {
        readAllButton.onClick();
    }

    writeAllButton.onClick = [this]
    {
        writeCommands(deviceSettings.getWriteCommands(true));
    };

    deviceSettings.onSettingModified = [this](const CommandMessage& command)
    {
        if (ApplicationSettings::getSingleton().deviceSettings.writeSettingsWhenModified)
        {
            writeCommands({ command });
        }
    };

    directory.createDirectory();

    saveToFileButton.onClick = [&]
    {
        auto fileName = deviceSettings.getValue("device_name").toString();
        if (fileName.isEmpty())
        {
            fileName = "Unknown Device";
        }
        if (const auto serialNumber = deviceSettings.getValue("serial_number").toString(); serialNumber.isNotEmpty())
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
            for (const auto& command : deviceSettings.getWriteCommands(false))
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
            if (file.getFileName().contains(deviceSettings.getValue("serial_number").toString()) && (file.getLastModificationTime() > defaultFile.value_or(juce::File()).getLastModificationTime()))
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
                    deviceSettings.setValue({ command.name.toString(), command.value });
                }
            }

            if (ApplicationSettings::getSingleton().deviceSettings.writeSettingsWhenModified)
            {
                writeCommands(deviceSettings.getWriteCommands(true));
            }
        });
    };

    defaultsButton.onClick = [this]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<AreYouSureDialog>("Are you sure you want to restore default device settings?"), [this]
        {
            enableInProgress(deviceSettings.getReadCommands());

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
}

DeviceSettingsWindow::~DeviceSettingsWindow()
{
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

    deviceSettings.setBounds(bounds);
    juce::ScopedValueSetter _(buttonBounds, buttonBounds);
    for (auto* const button : buttons)
    {
        button->setBounds(buttonBounds.removeFromLeft(buttonWidth).reduced(2.0f).toNearestInt());
    }
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
                deviceSettings.setStatus(command.key, Setting::Status::warning, "Unable to Write to Device");
                continue;
            }

            deviceSettings.setValue(*response);
            deviceSettings.setStatus(response->key, Setting::Status::normal, {});
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
        deviceSettings.setStatus(command.key, Setting::Status::normal, {});
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

    menu.addItem("Hide Unused Settings", true, ApplicationSettings::getSingleton().deviceSettings.hideUnusedSettings, [&]
    {
        ApplicationSettings::getSingleton().deviceSettings.hideUnusedSettings = !ApplicationSettings::getSingleton().deviceSettings.hideUnusedSettings;
    });
    menu.addItem("Read Settings When Window Opens", true, ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens, [&]
    {
        ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens = !ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens;
    });
    menu.addItem("Write Settings When Modified", true, ApplicationSettings::getSingleton().deviceSettings.writeSettingsWhenModified, [&]
    {
        ApplicationSettings::getSingleton().deviceSettings.writeSettingsWhenModified = !ApplicationSettings::getSingleton().deviceSettings.writeSettingsWhenModified;
    });

    return menu;
}
