#pragma once

#include "../Widgets/CustomTextEditor.h"
#include "../Widgets/CustomToggleButton.h"
#include "../Widgets/SimpleLabel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class ApplicationSettingsGroup : public juce::GroupComponent
{
public:
    ApplicationSettingsGroup(const juce::String& name, const int numberOfRows);

    juce::Rectangle<int> getContentBounds() const;

protected:
    static constexpr int rowMargin = 7;

private:
    static constexpr int verticalMargin = 15;
    static constexpr int topExtraMargin = 7;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApplicationSettingsGroup)
};

class SearchForConnectionsGroup : public ApplicationSettingsGroup
{
public:
    SearchForConnectionsGroup();

    void resized() override;

private:
    CustomToggleButton showOnStartupToggle { "Show on Startup" };
    CustomToggleButton usbToggle { "USB" };
    CustomToggleButton serialToggle { "Serial" };
    CustomToggleButton tcpToggle { "TCP" };
    CustomToggleButton udpToggle { "UDP" };
    CustomToggleButton bluetoothToggle { "Bluetooth" };

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

    CustomToggleButton closeSendingCommandDialogWhenCompleteButton { "Close Sending Command Dialog When Complete" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandsGroup)
};

class DeviceSettingsGroup : public ApplicationSettingsGroup
{
public:
    DeviceSettingsGroup();

    void resized() override;

private:
    CustomToggleButton readSettingsWhenWindowOpens { "Read Settings When Window Opens" };
    CustomToggleButton hideUnusedSettingsButton { "Hide Unused Settings" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettingsGroup)
};
