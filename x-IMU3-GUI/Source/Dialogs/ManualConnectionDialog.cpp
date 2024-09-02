#include "CustomLookAndFeel.h"
#include "ManualConnectionDialog.h"

ManualConnectionDialog::ManualConnectionDialog(const juce::String& dialogTitle) : Dialog(BinaryData::manual_svg, dialogTitle, "Connect")
{
}

ManualConnectionDialog::~ManualConnectionDialog()
{
}

ManualUsbConnectionDialog::ManualUsbConnectionDialog() : ManualConnectionDialog("Manual USB Connection")
{
    addAndMakeVisible(portNameLabel);
    addAndMakeVisible(portNameValue);

    setSize(dialogWidth, calculateHeight(1));
}

ManualUsbConnectionDialog::~ManualUsbConnectionDialog()
{
}

void ManualUsbConnectionDialog::resized()
{
    ManualConnectionDialog::resized();
    auto bounds = getContentBounds();
    auto portRow = bounds.removeFromTop(UILayout::textComponentHeight);
    portNameLabel.setBounds(portRow.removeFromLeft(columnWidth));
    portNameValue.setBounds(portRow);
}

std::unique_ptr<ximu3::ConnectionInfo> ManualUsbConnectionDialog::getConnectionInfo() const
{
    return std::make_unique<ximu3::UsbConnectionInfo>(portNameValue.getSelectedPortName());
}

ManualSerialConnectionDialog::ManualSerialConnectionDialog() : ManualConnectionDialog("Manual Serial Connection")
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

ManualSerialConnectionDialog::~ManualSerialConnectionDialog()
{
}

void ManualSerialConnectionDialog::resized()
{
    ManualConnectionDialog::resized();
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

std::unique_ptr<ximu3::ConnectionInfo> ManualSerialConnectionDialog::getConnectionInfo() const
{
    return std::make_unique<ximu3::SerialConnectionInfo>(portNameValue.getSelectedPortName(),
                                                         (uint32_t) baudRateValue.getText().getIntValue(),
                                                         rtsCtsEnabledToggle.getToggleState());
}

void ManualSerialConnectionDialog::validateDialog()
{
    setOkButton(baudRateValue.getText().isNotEmpty());
}

ManualTcpConnectionDialog::ManualTcpConnectionDialog() : ManualConnectionDialog("Manual TCP Connection")
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

ManualTcpConnectionDialog::~ManualTcpConnectionDialog()
{
}

void ManualTcpConnectionDialog::resized()
{
    ManualConnectionDialog::resized();
    auto bounds = getContentBounds();

    auto ipAddressRow = bounds.removeFromTop(UILayout::textComponentHeight);
    ipAddressLabel.setBounds(ipAddressRow.removeFromLeft(columnWidth));
    ipAddressValue.setBounds(ipAddressRow.removeFromLeft(2 * columnWidth));

    bounds.removeFromTop(Dialog::margin);

    auto portRow = bounds.removeFromTop(UILayout::textComponentHeight);
    portLabel.setBounds(portRow.removeFromLeft(columnWidth));
    portValue.setBounds(portRow.removeFromLeft(columnWidth));
}

std::unique_ptr<ximu3::ConnectionInfo> ManualTcpConnectionDialog::getConnectionInfo() const
{
    return std::make_unique<ximu3::TcpConnectionInfo>(ipAddressValue.getText().toStdString(),
                                                      (uint16_t) portValue.getText().getIntValue());
}

void ManualTcpConnectionDialog::validateDialog()
{
    setOkButton((ipAddressValue.isEmpty() || portValue.isEmpty()) == false);
}

ManualUdpConnectionDialog::ManualUdpConnectionDialog() : ManualConnectionDialog("Manual UDP Connection")
{
    addAndMakeVisible(ipAddressLabel);
    addAndMakeVisible(ipAddressValue);
    addAndMakeVisible(ipAddressBroadcastValue);
    addAndMakeVisible(sendPortLabel);
    addAndMakeVisible(sendPortValue);
    addAndMakeVisible(receivePortLabel);
    addAndMakeVisible(receivePortValue);
    addAndMakeVisible(receivePortFromIPValue);
    addAndMakeVisible(broadcastToggle);
    addAndMakeVisible(fromIpAddressToggle);

    ipAddressValue.onTextChange = broadcastToggle.onClick = [this]
    {
        ipAddressValue.setVisible(broadcastToggle.getToggleState() == false);
        ipAddressBroadcastValue.setVisible(broadcastToggle.getToggleState());
        receivePortFromIPValue.setText(juce::String(8000 + (broadcastToggle.getToggleState() ? 0 : juce::IPAddress(getIpAddressValue()).address[3])));
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

    fromIpAddressToggle.onClick = [this]
    {
        receivePortValue.setVisible(fromIpAddressToggle.getToggleState() == false);
        receivePortFromIPValue.setVisible(fromIpAddressToggle.getToggleState());
        validateDialog();
    };

    ipAddressValue.setText("192.168.1.1");
    ipAddressBroadcastValue.setText("255.255.255.255");
    ipAddressBroadcastValue.setEnabled(false);
    sendPortValue.setText("9000");
    receivePortValue.setText("8000");
    receivePortFromIPValue.setEnabled(false);
    fromIpAddressToggle.setToggleState(true, juce::sendNotificationSync);

    validateDialog();

    setSize(dialogWidth, calculateHeight(3));
}

ManualUdpConnectionDialog::~ManualUdpConnectionDialog()
{
}

void ManualUdpConnectionDialog::resized()
{
    ManualConnectionDialog::resized();
    auto bounds = getContentBounds();

    auto ipAddressRow = bounds.removeFromTop(UILayout::textComponentHeight);
    ipAddressLabel.setBounds(ipAddressRow.removeFromLeft(columnWidth));
    ipAddressValue.setBounds(ipAddressRow.removeFromLeft(2 * columnWidth));
    ipAddressBroadcastValue.setBounds(ipAddressValue.getBounds());
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
    receivePortFromIPValue.setBounds(receivePortValue.getBounds());
    receivePortRow.removeFromLeft(margin);
    fromIpAddressToggle.setBounds(receivePortRow);
}

std::unique_ptr<ximu3::ConnectionInfo> ManualUdpConnectionDialog::getConnectionInfo() const
{
    return std::make_unique<ximu3::UdpConnectionInfo>(getIpAddressValue().toStdString(),
                                                      (uint16_t) sendPortValue.getText().getIntValue(),
                                                      (uint16_t) getReceivePortValue().getIntValue());
}

juce::String ManualUdpConnectionDialog::getIpAddressValue() const
{
    return (broadcastToggle.getToggleState() ? ipAddressBroadcastValue : ipAddressValue).getText();
}

juce::String ManualUdpConnectionDialog::getReceivePortValue() const
{
    return (fromIpAddressToggle.getToggleState() ? receivePortFromIPValue : receivePortValue).getText();
}

void ManualUdpConnectionDialog::validateDialog()
{
    setOkButton((getIpAddressValue().isEmpty() || sendPortValue.isEmpty() || getReceivePortValue().isEmpty()) == false);
}

ManualBluetoothConnectionDialog::ManualBluetoothConnectionDialog() : ManualConnectionDialog("Manual Bluetooth Connection")
{
    addAndMakeVisible(portNameLabel);
    addAndMakeVisible(portNameValue);

    setSize(dialogWidth, calculateHeight(1));
}

ManualBluetoothConnectionDialog::~ManualBluetoothConnectionDialog()
{
}

void ManualBluetoothConnectionDialog::resized()
{
    ManualConnectionDialog::resized();
    auto bounds = getContentBounds();
    auto portRow = bounds.removeFromTop(UILayout::textComponentHeight);
    portNameLabel.setBounds(portRow.removeFromLeft(columnWidth));
    portNameValue.setBounds(portRow);
}

std::unique_ptr<ximu3::ConnectionInfo> ManualBluetoothConnectionDialog::getConnectionInfo() const
{
    return std::make_unique<ximu3::BluetoothConnectionInfo>(portNameValue.getSelectedPortName());
}
