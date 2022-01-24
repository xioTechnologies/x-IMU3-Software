#include "CustomLookAndFeel.h"
#include "NewConnectionDialog.h"

NewConnectionDialog::NewConnectionDialog(const juce::String& dialogTitle) : Dialog(BinaryData::manual_svg, dialogTitle, "Connect")
{
}

NewConnectionDialog::~NewConnectionDialog()
{
}

UsbConnectionDialog::UsbConnectionDialog() : NewConnectionDialog("New USB Connection")
{
    addAndMakeVisible(portNameLabel);
    addAndMakeVisible(portNameValue);

    setSize(dialogWidth, calculateHeight(1));
}

UsbConnectionDialog::~UsbConnectionDialog()
{
}

void UsbConnectionDialog::resized()
{
    NewConnectionDialog::resized();
    auto bounds = getContentBounds();
    auto portRow = bounds.removeFromTop(UILayout::textComponentHeight);
    portNameLabel.setBounds(portRow.removeFromLeft(columnWidth));
    portNameValue.setBounds(portRow);
}

std::unique_ptr<ximu3::ConnectionInfo> UsbConnectionDialog::getConnectionInfo() const
{
    return std::make_unique<ximu3::UsbConnectionInfo>(portNameValue.getSelectedPortName());
}

SerialConnectionDialog::SerialConnectionDialog() : NewConnectionDialog("New Serial Connection")
{
    addAndMakeVisible(portNameLabel);
    addAndMakeVisible(portNameValue);
    addAndMakeVisible(baudRateLabel);
    addAndMakeVisible(baudRateValue);
    addAndMakeVisible(rtsCtsEnabledToggle);

    portNameValue.onChange = [this]
    {
        validateDialog();
    };

    baudRateValue.onChange = [this]
    {
        validateDialog();
    };

    baudRateValue.addItemList({ "9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600" }, 1);
    baudRateValue.setSelectedItemIndex(4);
    baudRateValue.setEditableText(true);

    validateDialog();

    setSize(dialogWidth, calculateHeight(2));
}

SerialConnectionDialog::~SerialConnectionDialog()
{
}

void SerialConnectionDialog::resized()
{
    NewConnectionDialog::resized();
    auto bounds = getContentBounds();

    auto portRow = bounds.removeFromTop(UILayout::textComponentHeight);
    portNameLabel.setBounds(portRow.removeFromLeft(columnWidth));
    portNameValue.setBounds(portRow);

    bounds.removeFromTop(Dialog::margin);

    auto baudRateRow = bounds.removeFromTop(UILayout::textComponentHeight);
    baudRateLabel.setBounds(baudRateRow.removeFromLeft(columnWidth));
    baudRateValue.setBounds(baudRateRow.removeFromLeft(columnWidth));
    baudRateRow.removeFromLeft(margin);
    rtsCtsEnabledToggle.setBounds(baudRateRow);
}

std::unique_ptr<ximu3::ConnectionInfo> SerialConnectionDialog::getConnectionInfo() const
{
    return std::make_unique<ximu3::SerialConnectionInfo>(portNameValue.getSelectedPortName(),
                                                         (uint32_t) baudRateValue.getText().getIntValue(),
                                                         rtsCtsEnabledToggle.getToggleState());
}

void SerialConnectionDialog::validateDialog()
{
    setValid(baudRateValue.getText().isNotEmpty());
}

TcpConnectionDialog::TcpConnectionDialog() : NewConnectionDialog("New TCP Connection")
{
    addAndMakeVisible(ipAddressLabel);
    addAndMakeVisible(ipAddressValue);
    addAndMakeVisible(portLabel);
    addAndMakeVisible(portValue);

    ipAddressValue.onTextChange = [this]
    {
        validateDialog();
    };

    portValue.onTextChange = [this]
    {
        validateDialog();
    };

    ipAddressValue.setText("192.168.1.1", false);
    portValue.setText("7000", false);

    validateDialog();

    setSize(dialogWidth, calculateHeight(2));
}

TcpConnectionDialog::~TcpConnectionDialog()
{
}

void TcpConnectionDialog::resized()
{
    NewConnectionDialog::resized();
    auto bounds = getContentBounds();

    auto ipAddressRow = bounds.removeFromTop(UILayout::textComponentHeight);
    ipAddressLabel.setBounds(ipAddressRow.removeFromLeft(columnWidth));
    ipAddressValue.setBounds(ipAddressRow.removeFromLeft(2 * columnWidth));

    bounds.removeFromTop(Dialog::margin);

    auto portRow = bounds.removeFromTop(UILayout::textComponentHeight);
    portLabel.setBounds(portRow.removeFromLeft(columnWidth));
    portValue.setBounds(portRow.removeFromLeft(columnWidth));
}

std::unique_ptr<ximu3::ConnectionInfo> TcpConnectionDialog::getConnectionInfo() const
{
    return std::make_unique<ximu3::TcpConnectionInfo>(ipAddressValue.getText().toStdString(),
                                                      (uint16_t) portValue.getText().getIntValue());
}

void TcpConnectionDialog::validateDialog()
{
    setValid((ipAddressValue.isEmpty() || portValue.isEmpty()) == false);
}

UdpConnectionDialog::UdpConnectionDialog() : NewConnectionDialog("New UDP Connection")
{
    addAndMakeVisible(ipAddressLabel);
    addAndMakeVisible(ipAddressValue);
    addAndMakeVisible(sendPortLabel);
    addAndMakeVisible(sendPortValue);
    addAndMakeVisible(receivePortLabel);
    addAndMakeVisible(receivePortValue);
    addAndMakeVisible(broadcastToggle);

    ipAddressValue.onTextChange = [this]
    {
        validateDialog();
    };

    sendPortValue.onTextChange = [this]
    {
        validateDialog();
    };

    receivePortValue.onTextChange = [this]
    {
        validateDialog();
    };

    broadcastToggle.onClick = [this]
    {
        if (broadcastToggle.getToggleState())
        {
            ipAddressWhenBroadcastDisabled = ipAddressValue.getText();
            ipAddressValue.setText("255.255.255.255", false);
        }
        else
        {
            ipAddressValue.setText(ipAddressWhenBroadcastDisabled, false);
        }

        ipAddressValue.setEnabled(!broadcastToggle.getToggleState());
    };

    ipAddressValue.setText("192.168.1.1", false);
    sendPortValue.setText("9000", false);
    receivePortValue.setText("8000", false);

    validateDialog();

    setSize(dialogWidth, calculateHeight(3));
}

UdpConnectionDialog::~UdpConnectionDialog()
{
}

void UdpConnectionDialog::resized()
{
    NewConnectionDialog::resized();
    auto bounds = getContentBounds();

    auto ipAddressRow = bounds.removeFromTop(UILayout::textComponentHeight);
    ipAddressLabel.setBounds(ipAddressRow.removeFromLeft(columnWidth));
    ipAddressValue.setBounds(ipAddressRow.removeFromLeft(2 * columnWidth));

    ipAddressRow.removeFromLeft(margin);
    broadcastToggle.setBounds(ipAddressRow);

    bounds.removeFromTop(Dialog::margin);

    auto sendPortRow = bounds.removeFromTop(UILayout::textComponentHeight);
    sendPortLabel.setBounds(sendPortRow.removeFromLeft(columnWidth));
    sendPortValue.setBounds(sendPortRow.removeFromLeft(columnWidth));

    bounds.removeFromTop(Dialog::margin);

    auto receivePortRow = bounds.removeFromTop(UILayout::textComponentHeight);
    receivePortLabel.setBounds(receivePortRow.removeFromLeft(columnWidth));
    receivePortValue.setBounds(receivePortRow.removeFromLeft(columnWidth));
}

std::unique_ptr<ximu3::ConnectionInfo> UdpConnectionDialog::getConnectionInfo() const
{
    return std::make_unique<ximu3::UdpConnectionInfo>(ipAddressValue.getText().toStdString(),
                                                      (uint16_t) sendPortValue.getText().getIntValue(),
                                                      (uint16_t) receivePortValue.getText().getIntValue());
}

void UdpConnectionDialog::validateDialog()
{
    setValid((ipAddressValue.isEmpty() || sendPortValue.isEmpty() || receivePortValue.isEmpty()) == false);
}

BluetoothConnectionDialog::BluetoothConnectionDialog() : NewConnectionDialog("New Bluetooth Connection")
{
    addAndMakeVisible(portNameLabel);
    addAndMakeVisible(portNameValue);

    setSize(dialogWidth, calculateHeight(1));
}

BluetoothConnectionDialog::~BluetoothConnectionDialog()
{
}

void BluetoothConnectionDialog::resized()
{
    NewConnectionDialog::resized();
    auto bounds = getContentBounds();
    auto portRow = bounds.removeFromTop(UILayout::textComponentHeight);
    portNameLabel.setBounds(portRow.removeFromLeft(columnWidth));
    portNameValue.setBounds(portRow);
}

std::unique_ptr<ximu3::ConnectionInfo> BluetoothConnectionDialog::getConnectionInfo() const
{
    return std::make_unique<ximu3::BluetoothConnectionInfo>(portNameValue.getSelectedPortName());
}
