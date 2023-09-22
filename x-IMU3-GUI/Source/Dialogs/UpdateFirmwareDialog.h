#pragma once

#include "Dialog.h"
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class UpdateFirmwareDialog : public Dialog
{
public:
    UpdateFirmwareDialog();

    void resized() override;

    std::unique_ptr<ximu3::ConnectionInfo> getConnectionInfo() const;

    juce::String getFileName() const;

private:
    std::vector<ximu3::XIMU3_Device> devices;
    ximu3::PortScanner portScanner {
            [this](const std::vector<ximu3::XIMU3_Device>& devices_)
            {
                auto self = SafePointer<juce::Component>(this);
                juce::MessageManager::callAsync([this, self, devices_]
                                                {
                                                    if (self == nullptr)
                                                    {
                                                        return;
                                                    }

                                                    devices.clear();
                                                    for (auto& device : devices_)
                                                    {
                                                        if (device.connection_type != ximu3::XIMU3_ConnectionTypeBluetooth)
                                                        {
                                                            devices.push_back(device);
                                                        }
                                                    }

                                                    const auto id = deviceValue.getSelectedId();

                                                    deviceValue.clear();

                                                    for (const auto& device : devices)
                                                    {
                                                        juce::String connectionInfo;

                                                        switch (device.connection_type)
                                                        {
                                                            case ximu3::XIMU3_ConnectionTypeUsb:
                                                                connectionInfo = ximu3::XIMU3_usb_connection_info_to_string(device.usb_connection_info);
                                                                break;
                                                            case ximu3::XIMU3_ConnectionTypeSerial:
                                                                connectionInfo = ximu3::XIMU3_serial_connection_info_to_string(device.serial_connection_info);
                                                                break;
                                                            case ximu3::XIMU3_ConnectionTypeBluetooth:
                                                            case ximu3::XIMU3_ConnectionTypeTcp:
                                                            case ximu3::XIMU3_ConnectionTypeUdp:
                                                            case ximu3::XIMU3_ConnectionTypeFile:
                                                                break;
                                                        }

                                                        deviceValue.addItem(juce::String(device.device_name) + " " + juce::String(device.serial_number) + " (" + connectionInfo + ")", 1 + deviceValue.getNumItems());
                                                    }

                                                    deviceValue.setSelectedId(std::max(1, id));
                                                });
            }
    };

    SimpleLabel deviceLabel { "Device:" };
    CustomComboBox deviceValue;

    SimpleLabel fileNameLabel { "File Name:" };
    CustomTextEditor fileNameValue;
    IconButton fileNameButton { BinaryData::open_svg, "Select Firmware File" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UpdateFirmwareDialog)
};
