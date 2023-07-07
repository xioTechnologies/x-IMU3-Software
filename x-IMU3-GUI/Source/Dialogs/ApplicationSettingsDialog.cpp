#include "../ApplicationSettings.h"
#include "ApplicationSettingsDialog.h"
#include "ApplicationSettingsGroups.h"

ApplicationSettingsDialog::ApplicationSettingsDialog() : Dialog(BinaryData::settings_svg, "Application Settings", "Close", "", &defaultsButton, iconButtonWidth)
{
    addAndMakeVisible(defaultsButton);

    initialiseGroups();

    defaultsButton.onClick = [&]
    {
        ApplicationSettings::getSingleton().restoreDefault();

        groups.clear();
        initialiseGroups();
        resized();
    };

    int height = 0;
    for (auto& group : groups)
    {
        height += group->getHeight() + margin;
    }
    setSize(dialogWidth, calculateHeight(0) + height);
}

void ApplicationSettingsDialog::resized()
{
    Dialog::resized();
    auto bounds = getContentBounds();
    for (auto& group : groups)
    {
        group->setBounds(bounds.removeFromTop(group->getHeight()));
        bounds.removeFromTop(margin);
    }
}

void ApplicationSettingsDialog::initialiseGroups()
{
    addAndMakeVisible(*groups.emplace_back(std::make_unique<SearchForConnectionsGroup>()));
    addAndMakeVisible(*groups.emplace_back(std::make_unique<CommandsGroup>()));
    addAndMakeVisible(*groups.emplace_back(std::make_unique<DeviceSettingsGroup>()));
}
