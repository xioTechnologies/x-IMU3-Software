#include "../ApplicationSettings.h"
#include "ApplicationSettingsGroups.h"

ApplicationSettingsGroup::ApplicationSettingsGroup(const juce::String& name, int numberOfRows) : juce::GroupComponent("", name)
{
    setSize(0, topExtraMargin + verticalMargin * 2 + numberOfRows * rowHeight);
}

juce::Rectangle<int> ApplicationSettingsGroup::getContentBounds() const
{
    return getLocalBounds().reduced(10, verticalMargin).withTrimmedTop(topExtraMargin);
}

SearchForConnectionsGroup::SearchForConnectionsGroup() : ApplicationSettingsGroup("Search for Connections", 2)
{
    addAndMakeVisible(showOnStartupToggle);
    addAndMakeVisible(searchUsbToggle);
    addAndMakeVisible(searchSerialToggle);
    addAndMakeVisible(searchTcpToggle);
    addAndMakeVisible(searchUdpToggle);
    addAndMakeVisible(searchBluetoothToggle);

    showOnStartupToggle.onClick = [this]
    {
        ApplicationSettings::getSingleton().showSearchForConnectionsOnStartup = showOnStartupToggle.getToggleState();
    };

    searchUsbToggle.onClick = [this]
    {
        ApplicationSettings::getSingleton().searchUsb = searchUsbToggle.getToggleState();
    };

    searchSerialToggle.onClick = [this]
    {
        ApplicationSettings::getSingleton().searchSerial = searchSerialToggle.getToggleState();
    };

    searchTcpToggle.onClick = [this]
    {
        ApplicationSettings::getSingleton().searchTcp = searchTcpToggle.getToggleState();
    };

    searchUdpToggle.onClick = [this]
    {
        ApplicationSettings::getSingleton().searchUdp = searchUdpToggle.getToggleState();
    };

    searchBluetoothToggle.onClick = [this]
    {
        ApplicationSettings::getSingleton().searchBluetooth = searchBluetoothToggle.getToggleState();
    };

    showOnStartupToggle.setToggleState(ApplicationSettings::getSingleton().showSearchForConnectionsOnStartup, juce::dontSendNotification);
    searchUsbToggle.setToggleState(ApplicationSettings::getSingleton().searchUsb, juce::dontSendNotification);
    searchSerialToggle.setToggleState(ApplicationSettings::getSingleton().searchSerial, juce::dontSendNotification);
    searchTcpToggle.setToggleState(ApplicationSettings::getSingleton().searchTcp, juce::dontSendNotification);
    searchUdpToggle.setToggleState(ApplicationSettings::getSingleton().searchUdp, juce::dontSendNotification);
    searchBluetoothToggle.setToggleState(ApplicationSettings::getSingleton().searchBluetooth, juce::dontSendNotification);
}

void SearchForConnectionsGroup::resized()
{
    auto bounds = getContentBounds();

    showOnStartupToggle.setBounds(bounds.removeFromTop(rowHeight));

    static constexpr int toggleWidth = 80;
    searchUsbToggle.setBounds(bounds.removeFromLeft(toggleWidth));
    searchSerialToggle.setBounds(bounds.removeFromLeft(toggleWidth));
    searchTcpToggle.setBounds(bounds.removeFromLeft(toggleWidth));
    searchUdpToggle.setBounds(bounds.removeFromLeft(toggleWidth));
    searchBluetoothToggle.setBounds(bounds);
}

CommandsGroup::CommandsGroup() : ApplicationSettingsGroup("Commands", 2)
{
    addAndMakeVisible(retriesLabel);
    addAndMakeVisible(retriesValue);
    addAndMakeVisible(timeoutLabel);
    addAndMakeVisible(timeoutValue);

    retriesValue.onTextChange = [this]
    {
        ApplicationSettings::getSingleton().retries = (uint32_t) retriesValue.getText().getIntValue();
    };

    timeoutValue.onTextChange = [this]
    {
        ApplicationSettings::getSingleton().timeout = (uint32_t) timeoutValue.getText().getIntValue();
    };

    retriesValue.setText(juce::String(ApplicationSettings::getSingleton().retries.get()), juce::dontSendNotification);
    timeoutValue.setText(juce::String(ApplicationSettings::getSingleton().timeout.get()), juce::dontSendNotification);
}

void CommandsGroup::resized()
{
    auto bounds = getContentBounds();

    bounds.removeFromLeft(6); // align with toggle checkbox, from CustomToggleButtonLookAndFeel

    const auto setBounds = [&](juce::Component& label, juce::Component& value)
    {
        auto retriesBounds = bounds.removeFromTop(rowHeight);
        label.setBounds(retriesBounds.removeFromLeft(100));
        retriesBounds.removeFromLeft(15);
        value.setBounds(retriesBounds.removeFromLeft(100).reduced(0, 2));
    };
    setBounds(retriesLabel, retriesValue);
    setBounds(timeoutLabel, timeoutValue);
}

MiscGroup::MiscGroup() : ApplicationSettingsGroup("Misc", 2)
{
    addAndMakeVisible(hideUnusedDeviceSettingsButton);
    addAndMakeVisible(closeSendingCommandDialogWhenCompleteButton);

    hideUnusedDeviceSettingsButton.onClick = [this]
    {
        ApplicationSettings::getSingleton().hideUnusedDeviceSettings = hideUnusedDeviceSettingsButton.getToggleState();
    };

    closeSendingCommandDialogWhenCompleteButton.onClick = [this]
    {
        ApplicationSettings::getSingleton().closeSendingCommandDialogWhenComplete = closeSendingCommandDialogWhenCompleteButton.getToggleState();
    };

    hideUnusedDeviceSettingsButton.setToggleState(ApplicationSettings::getSingleton().hideUnusedDeviceSettings, juce::dontSendNotification);
    closeSendingCommandDialogWhenCompleteButton.setToggleState(ApplicationSettings::getSingleton().closeSendingCommandDialogWhenComplete, juce::dontSendNotification);
}

void MiscGroup::resized()
{
    auto bounds = getContentBounds();
    hideUnusedDeviceSettingsButton.setBounds(bounds.removeFromTop(rowHeight));
    closeSendingCommandDialogWhenCompleteButton.setBounds(bounds.removeFromTop(rowHeight));
}
