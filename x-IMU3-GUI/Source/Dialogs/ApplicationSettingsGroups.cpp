#include "ApplicationSettings.h"
#include "ApplicationSettingsGroups.h"

ApplicationSettingsGroup::ApplicationSettingsGroup(const juce::String& name, const int numberOfRows) : juce::GroupComponent("", name)
{
    setSize(0, topExtraMargin + verticalMargin * 2 + numberOfRows * UILayout::textComponentHeight + (numberOfRows - 1) * rowMargin);
}

juce::Rectangle<int> ApplicationSettingsGroup::getContentBounds() const
{
    return getLocalBounds().reduced(10, verticalMargin).withTrimmedTop(topExtraMargin);
}

SearchForConnectionsGroup::SearchForConnectionsGroup() : ApplicationSettingsGroup("Search for Connections", 2)
{
    addAndMakeVisible(showOnStartupToggle);
    addAndMakeVisible(usbToggle);
    addAndMakeVisible(serialToggle);
    addAndMakeVisible(tcpToggle);
    addAndMakeVisible(udpToggle);
    addAndMakeVisible(bluetoothToggle);

    showOnStartupToggle.onClick = [this]
    {
        ApplicationSettings::getSingleton().searchForConnections.showOnStartup = showOnStartupToggle.getToggleState();
    };

    usbToggle.onClick = [this]
    {
        ApplicationSettings::getSingleton().searchForConnections.usb = usbToggle.getToggleState();
    };

    serialToggle.onClick = [this]
    {
        ApplicationSettings::getSingleton().searchForConnections.serial = serialToggle.getToggleState();
    };

    tcpToggle.onClick = [this]
    {
        ApplicationSettings::getSingleton().searchForConnections.tcp = tcpToggle.getToggleState();
    };

    udpToggle.onClick = [this]
    {
        ApplicationSettings::getSingleton().searchForConnections.udp = udpToggle.getToggleState();
    };

    bluetoothToggle.onClick = [this]
    {
        ApplicationSettings::getSingleton().searchForConnections.bluetooth = bluetoothToggle.getToggleState();
    };

    showOnStartupToggle.setToggleState(ApplicationSettings::getSingleton().searchForConnections.showOnStartup, juce::dontSendNotification);
    usbToggle.setToggleState(ApplicationSettings::getSingleton().searchForConnections.usb, juce::dontSendNotification);
    serialToggle.setToggleState(ApplicationSettings::getSingleton().searchForConnections.serial, juce::dontSendNotification);
    tcpToggle.setToggleState(ApplicationSettings::getSingleton().searchForConnections.tcp, juce::dontSendNotification);
    udpToggle.setToggleState(ApplicationSettings::getSingleton().searchForConnections.udp, juce::dontSendNotification);
    bluetoothToggle.setToggleState(ApplicationSettings::getSingleton().searchForConnections.bluetooth, juce::dontSendNotification);
}

void SearchForConnectionsGroup::resized()
{
    auto bounds = getContentBounds();

    showOnStartupToggle.setBounds(bounds.removeFromTop(UILayout::textComponentHeight));
    bounds.removeFromTop(rowMargin);

    static constexpr int toggleWidth = 80;
    usbToggle.setBounds(bounds.removeFromLeft(toggleWidth));
    serialToggle.setBounds(bounds.removeFromLeft(toggleWidth));
    tcpToggle.setBounds(bounds.removeFromLeft(toggleWidth));
    udpToggle.setBounds(bounds.removeFromLeft(toggleWidth));
    bluetoothToggle.setBounds(bounds);
}

CommandsGroup::CommandsGroup() : ApplicationSettingsGroup("Commands", 3)
{
    addAndMakeVisible(retriesLabel);
    addAndMakeVisible(retriesValue);
    addAndMakeVisible(timeoutLabel);
    addAndMakeVisible(timeoutValue);
    addAndMakeVisible(closeSendingCommandDialogWhenCompleteButton);

    retriesValue.onTextChange = [this]
    {
        ApplicationSettings::getSingleton().commands.retries = (uint32_t) retriesValue.getText().getIntValue();
    };

    timeoutValue.onTextChange = [this]
    {
        ApplicationSettings::getSingleton().commands.timeout = (uint32_t) timeoutValue.getText().getIntValue();
    };

    closeSendingCommandDialogWhenCompleteButton.onClick = [this]
    {
        ApplicationSettings::getSingleton().commands.closeSendingCommandDialogWhenComplete = closeSendingCommandDialogWhenCompleteButton.getToggleState();
    };

    retriesValue.setText(juce::String(ApplicationSettings::getSingleton().commands.retries.get()), juce::dontSendNotification);
    timeoutValue.setText(juce::String(ApplicationSettings::getSingleton().commands.timeout.get()), juce::dontSendNotification);
    closeSendingCommandDialogWhenCompleteButton.setToggleState(ApplicationSettings::getSingleton().commands.closeSendingCommandDialogWhenComplete, juce::dontSendNotification);
}

void CommandsGroup::resized()
{
    auto bounds = getContentBounds();

    const auto setTextSettingBounds = [&](auto& label, auto& value)
    {
        auto row = bounds.removeFromTop(UILayout::textComponentHeight).withTrimmedLeft(6); // align with toggle checkbox, from CustomToggleButtonLookAndFeel
        label.setBounds(row.removeFromLeft(100));
        row.removeFromLeft(15);
        value.setBounds(row.removeFromLeft(100));
        bounds.removeFromTop(rowMargin);
    };

    setTextSettingBounds(retriesLabel, retriesValue);
    setTextSettingBounds(timeoutLabel, timeoutValue);

    closeSendingCommandDialogWhenCompleteButton.setBounds(bounds.removeFromTop(UILayout::textComponentHeight));
}

DeviceSettingsGroup::DeviceSettingsGroup() : ApplicationSettingsGroup("Device Settings", 2)
{
    addAndMakeVisible(readSettingsWhenWindowOpens);
    addAndMakeVisible(hideUnusedSettingsButton);

    readSettingsWhenWindowOpens.onClick = [this]
    {
        ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens = readSettingsWhenWindowOpens.getToggleState();
    };

    hideUnusedSettingsButton.onClick = [this]
    {
        ApplicationSettings::getSingleton().deviceSettings.hideUnusedSettings = hideUnusedSettingsButton.getToggleState();
    };

    readSettingsWhenWindowOpens.setToggleState(ApplicationSettings::getSingleton().deviceSettings.readSettingsWhenWindowOpens, juce::dontSendNotification);
    hideUnusedSettingsButton.setToggleState(ApplicationSettings::getSingleton().deviceSettings.hideUnusedSettings, juce::dontSendNotification);
}

void DeviceSettingsGroup::resized()
{
    auto bounds = getContentBounds();

    readSettingsWhenWindowOpens.setBounds(bounds.removeFromTop(UILayout::textComponentHeight));
    bounds.removeFromTop(rowMargin);
    hideUnusedSettingsButton.setBounds(bounds.removeFromTop(UILayout::textComponentHeight));
}
