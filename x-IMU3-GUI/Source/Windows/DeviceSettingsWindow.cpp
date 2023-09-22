#include "DevicePanel/DevicePanel.h"
#include "DeviceSettingsWindow.h"
#include "Dialogs/AreYouSureDialog.h"
#include "Dialogs/ErrorDialog.h"

DeviceSettingsWindow::DeviceSettingsWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, DevicePanel& devicePanel_)
        : Window(windowLayout_, type_, devicePanel_, "Device Settings Menu", std::bind(&DeviceSettingsWindow::getMenu, this))
{
    addAndMakeVisible(settingsTree);
    addAndMakeVisible(readAllButton);
    addAndMakeVisible(writeAllButton);
    addAndMakeVisible(saveToFileButton);
    addAndMakeVisible(loadFromFileButton);
    addAndMakeVisible(defaultsButton);

    readAllButton.onClick = [this]
    {
        enableInProgress(settingsTree.getReadCommands());

        devicePanel.sendCommands(settingsTree.getReadCommands(), this, [&](const std::vector<CommandMessage>& responses, const std::vector<CommandMessage>& failedCommands)
        {
            for (const auto& response : responses)
            {
                settingsTree.setValue(response);
                settingsTree.setStatus(response.key, Setting::Status::normal);
            }

            for (const auto& failedCommand : failedCommands)
            {
                settingsTree.setStatus(failedCommand.key, Setting::Status::readFailed);
            }

            readAllButton.setToggleState(failedCommands.empty() == false, juce::dontSendNotification);

            disableInProgress();
        });
    };

    if (ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens)
    {
        readAllButton.onClick();
    }

    writeAllButton.onClick = [this]
    {
        enableInProgress(settingsTree.getWriteCommands());

        devicePanel.sendCommands(settingsTree.getWriteCommands(), this, [&](const auto& responses, const auto& writeFailedCommands)
        {
            for (const auto& response : responses)
            {
                settingsTree.setValue(response);
                settingsTree.setStatus(response.key, Setting::Status::normal);
            }

            for (const auto& failedCommand : writeFailedCommands)
            {
                settingsTree.setStatus(failedCommand.key, Setting::Status::writeFailed);
            }

            writeAllButton.setToggleState(writeFailedCommands.empty() == false, juce::dontSendNotification);

            disableInProgress();

            devicePanel.sendCommands({{ "save", {}}}, this, [&](const auto&, const auto& saveFailedCommands)
            {
                if (saveFailedCommands.empty() == false)
                {
                    DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Unable to confirm save command."));
                    return;
                }

                devicePanel.sendCommands({{ "apply", {}}});
            });
        });
    };

    directory.createDirectory();

    saveToFileButton.onClick = [&]
    {
        auto fileName = settingsTree.getValue("deviceName").toString();
        if (fileName.isEmpty())
        {
            fileName = "Unknown Device";
        }
        if (const auto serialNumber = settingsTree.getValue("serialNumber").toString(); serialNumber.isNotEmpty())
        {
            fileName += " " + serialNumber;
        }

        juce::FileChooser fileChooser("Save Device Settings", directory.getChildFile(fileName), "*.json");
        if (fileChooser.browseForFileToSave(true) == false)
        {
            return;
        }

        juce::DynamicObject object;
        for (const auto& command : settingsTree.getWriteCommands(false))
        {
            object.setProperty(command.key, command.value);
        }

        juce::FileOutputStream stream(fileChooser.getResult());
        stream.setPosition(0);
        stream.truncate();
        object.writeAsJSON(stream, 0, false, 15);
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
                settingsTree.setValue({ command.name.toString(), command.value });
            }
        }
    };

    defaultsButton.onClick = [this]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<AreYouSureDialog>("Are you sure you want to restore default device settings?"), [this]
        {
            enableInProgress(settingsTree.getReadCommands());

            devicePanel.sendCommands({{ "default", {}}}, this, [this](const auto&, const auto& defaultFailedCommands)
            {
                if (defaultFailedCommands.empty() == false)
                {
                    disableInProgress();
                    DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Unable to confirm default command."));
                    return;
                }

                devicePanel.sendCommands({{ "save", {}}}, this, [this](const auto&, const auto& saveFailedCommands)
                {
                    if (saveFailedCommands.empty() == false)
                    {
                        disableInProgress();
                        DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Unable to confirm save command."));
                        return;
                    }

                    devicePanel.sendCommands({{ "apply", {}}}, this, [this](const auto&, const auto&)
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

    settingsTree.setBounds(bounds);
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
        settingsTree.setStatus(command.key, Setting::Status::normal);
    }
    readAllButton.setToggleState(false, juce::dontSendNotification);
    writeAllButton.setToggleState(false, juce::dontSendNotification);

    settingsTree.setEnabled(false);
    for (auto* const button : buttons)
    {
        button->setEnabled(false);
    }
}

void DeviceSettingsWindow::disableInProgress()
{
    settingsTree.setEnabled(true);
    for (auto* const button : buttons)
    {
        button->setEnabled(true);
    }
}

juce::PopupMenu DeviceSettingsWindow::getMenu()
{
    juce::PopupMenu menu;

    menu.addItem("Read Settings When Window Opens", true, ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens, [&]
    {
        ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens = !ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens;
    });

    return menu;
}
