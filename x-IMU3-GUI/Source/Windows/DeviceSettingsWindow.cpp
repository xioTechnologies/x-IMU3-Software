#include "../DevicePanel/DevicePanel.h"
#include "DeviceSettingsWindow.h"
#include "Dialogs/AreYouSureDialog.h"
#include "Dialogs/ErrorDialog.h"

DeviceSettingsWindow::DeviceSettingsWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_) : Window(windowLayout, type, devicePanel_)
{
    addAndMakeVisible(settingsTree);
    addAndMakeVisible(readAllButton);
    addChildComponent(readAllFailedButton);
    addAndMakeVisible(writeAllButton);
    addChildComponent(writeAllFailedButton);
    addAndMakeVisible(saveToFileButton);
    addAndMakeVisible(loadFromFileButton);
    addAndMakeVisible(defaultsButton);

    readAllButton.onClick = readAllFailedButton.onClick = [this]
    {
        readAllButton.setVisible(true);
        readAllFailedButton.setVisible(false);
        writeAllButton.setVisible(true);
        writeAllFailedButton.setVisible(false);

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

            readAllButton.setVisible(failedCommands.empty());
            readAllFailedButton.setVisible(failedCommands.empty() == false);
        });
    };

    readAllButton.onClick();

    writeAllButton.onClick = writeAllFailedButton.onClick = [this]
    {
        readAllButton.setVisible(true);
        readAllFailedButton.setVisible(false);
        writeAllButton.setVisible(true);
        writeAllFailedButton.setVisible(false);

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

            writeAllButton.setVisible(writeFailedCommands.empty());
            writeAllFailedButton.setVisible(writeFailedCommands.empty() == false);

            devicePanel.sendCommands({{ "save", {}}}, this, [&](const auto&, const auto& saveFailedCommands)
            {
                if (saveFailedCommands.empty() == false)
                {
                    DialogLauncher::launchDialog(std::make_unique<ErrorDialog>("Write settings to device failed. Unable to confirm save command."));
                    return;
                }

                devicePanel.sendCommands({{ "apply", {}}}, this, [](const auto&, const auto& applyFailedCommands)
                {
                    if (applyFailedCommands.empty() == false)
                    {
                        DialogLauncher::launchDialog(std::make_unique<ErrorDialog>("Write settings to device failed. Unable to confirm apply command."));
                    }
                });
            });
        });
    };

    directory.createDirectory();

    saveToFileButton.onClick = [&]
    {
        juce::FileChooser fileChooser("Save Device Settings", directory.getChildFile("Device Settings"), "*.json");
        if (fileChooser.browseForFileToSave(true) == false)
        {
            return;
        }

        juce::DynamicObject object;
        for (const auto& command : settingsTree.getWriteCommands())
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
        DialogLauncher::launchDialog(std::make_unique<AreYouSureDialog>("Are you sure you want to restore default settings?"), [this]
        {
            devicePanel.sendCommands({{ "default", {}}}, this, [this](const auto&, const auto& defaultFailedCommands)
            {
                if (defaultFailedCommands.empty() == false)
                {
                    DialogLauncher::launchDialog(std::make_unique<ErrorDialog>("Restore default settings failed. Unable to confirm default command."));
                    return;
                }

                devicePanel.sendCommands({{ "save", {}}}, this, [this](const auto&, const auto& saveFailedCommands)
                {
                    if (saveFailedCommands.empty() == false)
                    {
                        DialogLauncher::launchDialog(std::make_unique<ErrorDialog>("Restore default settings failed. Unable to confirm save command."));
                        return;
                    }

                    devicePanel.sendCommands({{ "apply", {}}}, this, [this](const auto&, const auto& applyFailedCommands)
                    {
                        if (applyFailedCommands.empty() == false)
                        {
                            DialogLauncher::launchDialog(std::make_unique<ErrorDialog>("Restore default settings failed. Unable to confirm apply command."));
                            return;
                        }

                        readAllButton.onClick();
                    });
                });
            });
        });
    };
}

DeviceSettingsWindow::~DeviceSettingsWindow()
{
}

void DeviceSettingsWindow::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::background);
    Window::paint(g);
}

void DeviceSettingsWindow::resized()
{
    Window::resized();

    auto bounds = getContentBounds();
    auto buttons = { &readAllButton, &writeAllButton, &saveToFileButton, &loadFromFileButton, &defaultsButton };
    auto buttonsBounds = bounds.removeFromBottom(25).toFloat();
    const auto buttonWidth = buttonsBounds.getWidth() / buttons.size();

    settingsTree.setBounds(bounds);

    for (auto button : buttons)
    {
        button->setBounds(buttonsBounds.removeFromLeft(buttonWidth).reduced(2.0f).toNearestInt());
    }

    readAllFailedButton.setBounds(readAllButton.getBounds());
    writeAllFailedButton.setBounds(writeAllButton.getBounds());
}
