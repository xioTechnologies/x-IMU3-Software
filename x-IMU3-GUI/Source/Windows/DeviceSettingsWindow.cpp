#include "ConnectionPanel/ConnectionPanel.h"
#include "DeviceSettingsWindow.h"
#include "Dialogs/AreYouSureDialog.h"
#include "Dialogs/ErrorDialog.h"

DeviceSettingsWindow::DeviceSettingsWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_)
        : Window(windowLayout_, type_, connectionPanel_, "Device Settings Menu")
{
    addAndMakeVisible(deviceSettings);
    addAndMakeVisible(readAllButton);
    addAndMakeVisible(writeAllButton);
    addAndMakeVisible(saveToFileButton);
    addAndMakeVisible(loadFromFileButton);
    addAndMakeVisible(defaultsButton);

    readAllButton.onClick = [this]
    {
        const auto commands = deviceSettings.getReadCommands();

        enableInProgress(commands);

        connectionPanel.sendCommands(commands, this, [&, commands](const std::vector<CommandMessage>& responses)
        {
            for (const auto& command : commands)
            {
                const auto response = std::find(responses.begin(), responses.end(), command);

                if (response == responses.end() || response->getError())
                {
                    deviceSettings.setStatus(command.key, Setting::Status::readFailed);
                    continue;
                }

                deviceSettings.setValue(*response);
                deviceSettings.setStatus(response->key, Setting::Status::normal);
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
        const auto commands = deviceSettings.getWriteCommands();

        enableInProgress(commands);

        connectionPanel.sendCommands(commands, this, [&, commands](const auto& responses)
        {
            for (const auto& command : commands)
            {
                const auto response = std::find(responses.begin(), responses.end(), command);

                if (response == responses.end() || response->getError())
                {
                    deviceSettings.setStatus(command.key, Setting::Status::writeFailed);
                    continue;
                }

                deviceSettings.setValue(*response);
                deviceSettings.setStatus(response->key, Setting::Status::normal);
            }

            writeAllButton.setToggleState(commands.size() != responses.size(), juce::dontSendNotification);

            disableInProgress();

            connectionPanel.sendCommands({{ "save", {}}}, this, [&](const auto& responses_)
            {
                if (responses_.empty())
                {
                    DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Unable to confirm save command."));
                    return;
                }

                connectionPanel.sendCommands({{ "apply", {}}});
            });
        });
    };

    directory.createDirectory();

    saveToFileButton.onClick = [&]
    {
        auto fileName = deviceSettings.getValue("deviceName").toString();
        if (fileName.isEmpty())
        {
            fileName = "Unknown Device";
        }
        if (const auto serialNumber = deviceSettings.getValue("serialNumber").toString(); serialNumber.isNotEmpty())
        {
            fileName += " " + serialNumber;
        }

        juce::FileChooser fileChooser("Save Device Settings", directory.getChildFile(fileName), "*.json");
        if (fileChooser.browseForFileToSave(true) == false)
        {
            return;
        }

        juce::DynamicObject object;
        for (const auto& command : deviceSettings.getWriteCommands(false))
        {
            object.setProperty(command.key, command.value);
        }

        juce::FileOutputStream stream(fileChooser.getResult());
        stream.setPosition(0);
        stream.truncate();
        object.writeAsJSON(stream, {});
    };

    loadFromFileButton.onClick = [&]
    {
        juce::FileChooser fileChooser("Load Device Settings", directory, "*.json");
        if (fileChooser.browseForFileToOpen() == false)
        {
            return;
        }

        const auto parsed = juce::JSON::parse(fileChooser.getResult().loadFileAsString());
        if (auto* const object = parsed.getDynamicObject())
        {
            for (const auto& command : object->getProperties())
            {
                deviceSettings.setValue({ command.name.toString(), command.value });
            }
        }
    };

    defaultsButton.onClick = [this]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<AreYouSureDialog>("Are you sure you want to restore default device settings?"), [this]
        {
            enableInProgress(deviceSettings.getReadCommands());

            connectionPanel.sendCommands({{ "default", {}}}, this, [this](const auto& responses)
            {
                if (responses.empty())
                {
                    disableInProgress();
                    DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Unable to confirm default command."));
                    return;
                }

                connectionPanel.sendCommands({{ "save", {}}}, this, [this](const auto& responses_)
                {
                    if (responses_.empty())
                    {
                        disableInProgress();
                        DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Unable to confirm save command."));
                        return;
                    }

                    connectionPanel.sendCommands({{ "apply", {}}}, this, [this](const auto&)
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
    buttonBounds = bounds.removeFromBottom(25).toFloat();
    const auto buttonWidth = buttonBounds.getWidth() / buttons.size();

    deviceSettings.setBounds(bounds);
    juce::ScopedValueSetter _(buttonBounds, buttonBounds);
    for (auto* const button : buttons)
    {
        button->setBounds(buttonBounds.removeFromLeft(buttonWidth).reduced(2.0f).toNearestInt());
    }
}

void DeviceSettingsWindow::enableInProgress(const std::vector<CommandMessage>& commands)
{
    for (const auto& command : commands)
    {
        deviceSettings.setStatus(command.key, Setting::Status::normal);
    }
    readAllButton.setToggleState(false, juce::dontSendNotification);
    writeAllButton.setToggleState(false, juce::dontSendNotification);

    deviceSettings.setEnabled(false);
    for (auto* const button : buttons)
    {
        button->setEnabled(false);
    }
}

void DeviceSettingsWindow::disableInProgress()
{
    deviceSettings.setEnabled(true);
    for (auto* const button : buttons)
    {
        button->setEnabled(true);
    }
}

juce::PopupMenu DeviceSettingsWindow::getMenu()
{
    juce::PopupMenu menu = Window::getMenu();

    menu.addItem("Read Settings When Window Opens", true, ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens, [&]
    {
        ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens = !ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens;
    });

    return menu;
}
