#pragma once

#include "ConnectionPanel/ConnectionPanel.h"
#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/CustomToggleButton.h"
#include "Widgets/PortNameComboBox.h"
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class ManualConnectionDialog : public Dialog
{
public:
    ~ManualConnectionDialog() override;

    virtual std::unique_ptr<ximu3::ConnectionInfo> getConnectionInfo() const = 0;

protected:
    ManualConnectionDialog(const juce::String& dialogTitle);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ManualConnectionDialog)
};

class ManualUsbConnectionDialog : public ManualConnectionDialog
{
public:
    ManualUsbConnectionDialog();

    ~ManualUsbConnectionDialog() override;

    void resized() override;

    std::unique_ptr<ximu3::ConnectionInfo> getConnectionInfo() const override;

private:
    SimpleLabel portNameLabel { "Port Name:" };
    PortNameComboBox portNameValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ManualUsbConnectionDialog)
};

class ManualSerialConnectionDialog : public ManualConnectionDialog
{
public:
    ManualSerialConnectionDialog();

    ~ManualSerialConnectionDialog() override;

    void resized() override;

    std::unique_ptr<ximu3::ConnectionInfo> getConnectionInfo() const override;

private:
    SimpleLabel portNameLabel { "Port Name:" };
    SimpleLabel baudRateLabel { "Baud Rate:" };

    PortNameComboBox portNameValue;
    CustomComboBox baudRateValue;
    CustomToggleButton rtsCtsEnabledToggle { "RTS/CTS Enabled" };

    void validateDialog();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ManualSerialConnectionDialog)
};

class ManualTcpConnectionDialog : public ManualConnectionDialog
{
public:
    ManualTcpConnectionDialog();

    ~ManualTcpConnectionDialog() override;

    void resized() override;

    std::unique_ptr<ximu3::ConnectionInfo> getConnectionInfo() const override;

private:
    SimpleLabel ipAddressLabel { "IP Address:" };
    SimpleLabel portLabel { "Port:" };

    CustomTextEditor ipAddressValue;
    CustomTextEditor portValue;

    void validateDialog();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ManualTcpConnectionDialog)
};

class ManualUdpConnectionDialog : public ManualConnectionDialog
{
public:
    ManualUdpConnectionDialog();

    ~ManualUdpConnectionDialog() override;

    void resized() override;

    std::unique_ptr<ximu3::ConnectionInfo> getConnectionInfo() const override;

private:
    SimpleLabel ipAddressLabel { "IP Address:" };
    SimpleLabel sendPortLabel { "Send Port:" };
    SimpleLabel receivePortLabel { "Receive Port:" };

    CustomTextEditor ipAddressValue;
    CustomTextEditor sendPortValue;
    CustomTextEditor receivePortValue;
    CustomToggleButton broadcastToggle { "Broadcast" };

    juce::String ipAddressWhenBroadcastDisabled;

    void validateDialog();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ManualUdpConnectionDialog)
};

class ManualBluetoothConnectionDialog : public ManualConnectionDialog
{
public:
    ManualBluetoothConnectionDialog();

    ~ManualBluetoothConnectionDialog() override;

    void resized() override;

    std::unique_ptr<ximu3::ConnectionInfo> getConnectionInfo() const override;

private:
    SimpleLabel portNameLabel { "Port Name:" };
    PortNameComboBox portNameValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ManualBluetoothConnectionDialog)
};
