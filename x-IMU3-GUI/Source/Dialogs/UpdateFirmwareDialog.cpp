#include "../ApplicationSettings.h"
#include "../Firmware/Firmware.h"
#include <filesystem>
#include "UpdateFirmwareDialog.h"

UpdateFirmwareDialog::UpdateFirmwareDialog() : Dialog(BinaryData::tools_svg, "Update Firmware", "Update")
{
    addAndMakeVisible(deviceLabel);
    addAndMakeVisible(deviceValue);
    addAndMakeVisible(fileNameLabel);
    addAndMakeVisible(fileNameValue);
    addAndMakeVisible(fileNameButton);

    const auto fileName = ApplicationSettings::getDirectory().getChildFile(Firmware::fileName);
    fileName.replaceWithData(Firmware::memoryBlock.getData(), Firmware::memoryBlock.getSize());
    fileNameValue.setText(fileName.getFullPathName());

    fileNameButton.onClick = [&]
    {
        juce::FileChooser fileChooser(fileNameButton.getTooltip(), std::filesystem::exists(fileNameValue.getText().toStdString()) ? fileNameValue.getText() : "", "*.hex");
        if (fileChooser.browseForFileToOpen())
        {
            fileNameValue.setText(fileChooser.getResult().getFullPathName());
        }
    };

    deviceValue.onChange = fileNameValue.onTextChange = [&]
    {
        setOkButton(getConnectionInfo() != nullptr && std::filesystem::exists(fileNameValue.getText().toStdString()));
    };
    setOkButton(false);

    setSize(600, calculateHeight(2));
}

void UpdateFirmwareDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds();
    auto portRow = bounds.removeFromTop(UILayout::textComponentHeight);
    deviceLabel.setBounds(portRow.removeFromLeft(columnWidth));
    deviceValue.setBounds(portRow);

    bounds.removeFromTop(Dialog::margin);

    auto sourceRow = bounds.removeFromTop(UILayout::textComponentHeight);
    fileNameLabel.setBounds(sourceRow.removeFromLeft(columnWidth));
    fileNameButton.setBounds(sourceRow.removeFromRight(50));
    fileNameValue.setBounds(sourceRow.withTrimmedRight(margin));
}

std::unique_ptr<ximu3::ConnectionInfo> UpdateFirmwareDialog::getConnectionInfo() const
{
    if (deviceValue.getSelectedId() == 0)
    {
        return nullptr;
    }

    const auto& device = devices[(size_t) deviceValue.getSelectedId() - 1];
    switch (device.connection_type)
    {
        case ximu3::XIMU3_ConnectionTypeUsb:
            return std::make_unique<ximu3::UsbConnectionInfo>(device.usb_connection_info);
        case ximu3::XIMU3_ConnectionTypeSerial:
            return std::make_unique<ximu3::SerialConnectionInfo>(device.serial_connection_info);
        case ximu3::XIMU3_ConnectionTypeBluetooth:
        case ximu3::XIMU3_ConnectionTypeTcp:
        case ximu3::XIMU3_ConnectionTypeUdp:
        case ximu3::XIMU3_ConnectionTypeFile:
            return nullptr;
    }

    return nullptr;
}

juce::String UpdateFirmwareDialog::getFileName() const
{
    return fileNameValue.getText();
}
