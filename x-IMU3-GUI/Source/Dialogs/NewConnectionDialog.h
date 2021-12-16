#pragma once

#include "DevicePanel/DevicePanel.h"
#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/CustomToggleButton.h"
#include "Widgets/SerialPortComboBox.h"
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class NewConnectionDialog : public Dialog
{
public:
    ~NewConnectionDialog() override;

    virtual std::unique_ptr<ximu3::ConnectionInfo> getConnectionInfo() const = 0;

protected:
    NewConnectionDialog(const juce::String& dialogTitle);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewConnectionDialog)
};

class UsbConnectionDialog : public NewConnectionDialog
{
public:
    UsbConnectionDialog();

    ~UsbConnectionDialog() override;

    void resized() override;

    std::unique_ptr<ximu3::ConnectionInfo> getConnectionInfo() const override;

private:
    SimpleLabel portNameLabel { "Port Name:" };
    SerialPortComboBox portNameValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UsbConnectionDialog)
};

class SerialConnectionDialog : public NewConnectionDialog
{
public:
    SerialConnectionDialog();

    ~SerialConnectionDialog() override;

    void resized() override;

    std::unique_ptr<ximu3::ConnectionInfo> getConnectionInfo() const override;

private:
    SimpleLabel portNameLabel { "Port Name:" };
    SimpleLabel baudRateLabel { "Baud Rate:" };

    SerialPortComboBox portNameValue;
    CustomComboBox baudRateValue;
    CustomToggleButton rtsCtsEnabledToggle { "RTS/CTS Enabled" };

    void validateDialog();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SerialConnectionDialog)
};

class TcpConnectionDialog : public NewConnectionDialog
{
public:
    TcpConnectionDialog();

    ~TcpConnectionDialog() override;

    void resized() override;

    std::unique_ptr<ximu3::ConnectionInfo> getConnectionInfo() const override;

private:
    SimpleLabel ipAddressLabel { "IP Address:" };
    SimpleLabel portLabel { "Port:" };

    CustomTextEditor ipAddressValue;
    CustomTextEditor portValue;

    void validateDialog();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TcpConnectionDialog)
};

class UdpConnectionDialog : public NewConnectionDialog
{
public:
    UdpConnectionDialog();

    ~UdpConnectionDialog() override;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UdpConnectionDialog)
};

class BluetoothConnectionDialog : public NewConnectionDialog
{
public:
    BluetoothConnectionDialog();

    ~BluetoothConnectionDialog() override;

    void resized() override;

    std::unique_ptr<ximu3::ConnectionInfo> getConnectionInfo() const override;

private:
    SimpleLabel portNameLabel { "Port Name:" };
    SerialPortComboBox portNameValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BluetoothConnectionDialog)
};
