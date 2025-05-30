#pragma once

#include "Dialog.h"
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/FileSelector.h"
#include "Widgets/Icon.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class UpdateFirmwareDialog : public Dialog
{
public:
    UpdateFirmwareDialog();

    void resized() override;

    std::shared_ptr<ximu3::ConnectionInfo> getConnectionInfo() const;

    juce::File getHexFile() const;

    static void launch(juce::ThreadPool& threadPool);

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
                    if (ximu3::connectionInfoFrom(device)->getType() != ximu3::XIMU3_ConnectionTypeBluetooth)
                    {
                        devices.push_back(device);
                    }
                }

                const auto id = deviceValue.getSelectedId();
                deviceValue.clear();
                for (const auto& device : devices)
                {
                    deviceValue.addItem(juce::String(device.device_name) + " " + juce::String(device.serial_number) + " (" + ximu3::connectionInfoFrom(device)->toString() + ")", 1 + deviceValue.getNumItems());
                }
                deviceValue.setSelectedId(std::max(1, id));
            });
        }
    };

    SimpleLabel deviceLabel { "Device:" };
    CustomComboBox deviceValue;

    SimpleLabel hexFileLabel { "Hex File:" };
    FileSelector hexFileSelector { "Select Firmware File", ".hex" };

    Icon warningIcon { BinaryData::warning_white_svg, "", 0.7f };
    SimpleLabel warningLabel { "Updating firmware will restore default settings" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UpdateFirmwareDialog)
};
