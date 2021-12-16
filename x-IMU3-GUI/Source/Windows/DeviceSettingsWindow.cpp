#include "../DevicePanel/DevicePanel.h"
#include "../Dialogs/ApplicationErrorsDialog.h"
#include "DeviceSettingsWindow.h"
#include "Dialogs/AreYouSureDialog.h"

DeviceSettingsWindow::DeviceSettingsWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_) : Window(windowLayout, type, devicePanel_)
{
    addAndMakeVisible(settingsTree);
    addAndMakeVisible(readAllButton);
    addAndMakeVisible(writeAllButton);
    addAndMakeVisible(saveToFileButton);
    addAndMakeVisible(loadFromFileButton);
    addAndMakeVisible(defaultsButton);

    sendCommandsCallback = [this, self = SafePointer<juce::Component>(this)](const auto& responses, const auto& failedCommands)
    {
        juce::MessageManager::callAsync([&, self, responses, failedCommands]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            for (const auto& response : responses)
                                            {
                                                settingsTree.setValue(response);
                                                settingsTree.setStatus(response.key, Setting::Status::normal);
                                            }

                                            for (const auto& failedCommand : failedCommands)
                                            {
                                                settingsTree.setStatus(failedCommand.key, Setting::Status::failed);
                                            }
                                        });
    };

    readAllButton.onClick = [this]
    {
        devicePanel.sendCommands(settingsTree.getReadCommands(), sendCommandsCallback);
    };

    readAllButton.onClick();

    writeAllButton.onClick = [this]
    {
        devicePanel.sendCommands(settingsTree.getWriteCommands(), [this](const auto& responses, const auto& failedCommands)
        {
            sendCommandsCallback(responses, failedCommands);

            devicePanel.sendCommands({{ "save", {}}}, [this](const auto&, const auto&)
            {
                devicePanel.sendCommands({{ "apply", {}}}, nullptr);
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
        DialogLauncher::launchDialog(std::make_unique<AreYouSureDialog>("Are you sure you want to restore default settings?"), [this, readCommands = settingsTree.getReadCommands()]
        {
            devicePanel.sendCommands({{ "default", {}}}, [this, readCommands](auto, auto)
            {
                devicePanel.sendCommands(readCommands, sendCommandsCallback);

                devicePanel.sendCommands({{ "save", {}}}, [this](auto, auto)
                {
                    devicePanel.sendCommands({{ "apply", {}}}, nullptr);
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
}
