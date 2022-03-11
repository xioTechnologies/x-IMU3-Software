#pragma once

#include "../Widgets/CustomTextEditor.h"
#include "../Widgets/CustomToggleButton.h"
#include "../Widgets/SimpleLabel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class ApplicationSettingsGroup : public juce::GroupComponent
{
public:
    static constexpr int rowHeight = 30;

    ApplicationSettingsGroup(const juce::String& name, int numberOfRows);

    juce::Rectangle<int> getContentBounds() const;

private:
    static constexpr int verticalMargin = 15;
    static constexpr int topExtraMargin = 7;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApplicationSettingsGroup)
};

class StartupGroup : public ApplicationSettingsGroup
{
public:
    StartupGroup();

    void resized() override;

private:
    CustomToggleButton searchForConnectionsToggle { "Search for Connections" };
    CustomToggleButton checkFirmwareVersionToggle { "Check Firmware Version" };
    CustomToggleButton checkDeviceTimeToggle { "Check Device Time" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StartupGroup)
};

class SearchForConnectionsGroup : public ApplicationSettingsGroup
{
public:
    SearchForConnectionsGroup();

    void resized() override;

private:
    CustomToggleButton searchUsbToggle { "USB" };
    CustomToggleButton searchSerialToggle { "Serial" };
    CustomToggleButton searchTcpToggle { "TCP" };
    CustomToggleButton searchUdpToggle { "UDP" };
    CustomToggleButton searchBluetoothToggle { "Bluetooth" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SearchForConnectionsGroup)
};

class CommandsGroup : public ApplicationSettingsGroup
{
public:
    CommandsGroup();

    void resized() override;

private:
    SimpleLabel retriesLabel { "Retries" };
    CustomTextEditor retriesValue;

    SimpleLabel timeoutLabel { "Timeout (ms)" };
    CustomTextEditor timeoutValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandsGroup)
};

class MiscGroup : public ApplicationSettingsGroup
{
public:
    MiscGroup();

    void resized() override;

private:
    CustomToggleButton hideUnusedDeviceSettingsButton { "Hide Unused Device Settings" };
    CustomToggleButton closeSendingCommandDialogWhenCompleteButton { "Close Sending Command Dialog When Complete" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MiscGroup)
};
