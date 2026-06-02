#include "ApplicationSettings.h"
#include "Firmware/Firmware.h"
#include "UpdateFirmwareDialog.h"
#include "UpdatingFirmwareDialog.h"

UpdateFirmwareDialog::UpdateFirmwareDialog() : Dialog(BinaryData::tools_svg, "Update Firmware", "Update", "Cancel", &warningIcon, iconButtonWidth) {
    addAndMakeVisible(deviceLabel);
    addAndMakeVisible(deviceValue);
    addAndMakeVisible(hexFileLabel);
    addAndMakeVisible(hexFileSelector);
    addAndMakeVisible(warningIcon);
    addAndMakeVisible(warningLabel);

    const auto hexFile = ApplicationSettings::getDirectory().getChildFile(Firmware::hexFile);
    hexFile.replaceWithData(Firmware::memoryBlock.getData(), Firmware::memoryBlock.getSize());
    hexFileSelector.setFiles({hexFile});

    deviceValue.onChange = hexFileSelector.onChange = [&] {
        setOkButton(getConnectionConfig() != nullptr && hexFileSelector.isValid());
    };
    setOkButton(false);

    portScannerCallback = [this](const std::vector<ximu3::Device> &devices_) {
        auto self = SafePointer<juce::Component>(this);
        juce::MessageManager::callAsync([this, self, devices_] {
            if (self == nullptr) {
                return;
            }

            devices.clear();
            for (auto &device: devices_) {
                const auto config = ximu3::ConnectionConfig::from(device);

                if ((dynamic_cast<ximu3::UsbConnectionConfig *>(config.get()) != nullptr) ||
                    (dynamic_cast<ximu3::SerialConnectionConfig *>(config.get()) != nullptr)) {
                    devices.push_back(device);
                }
            }

            const auto id = deviceValue.getSelectedId();
            deviceValue.clear();
            for (const auto &device: devices) {
                deviceValue.addItem(juce::String(device.device_name) + " " + juce::String(device.serial_number) + " (" + ximu3::ConnectionConfig::from(device)->toString() + ")", 1 + deviceValue.getNumItems());
            }
            deviceValue.setSelectedId(std::max(1, id));
        });
    };

    portScanner.addCallback(portScannerCallback);

    setSize(600, calculateHeight(2));
}

void UpdateFirmwareDialog::resized() {
    Dialog::resized();

    auto bounds = getContentBounds();

    auto deviceRow = bounds.removeFromTop(UILayout::textComponentHeight);
    deviceLabel.setBounds(deviceRow.removeFromLeft(columnWidth));
    deviceValue.setBounds(deviceRow);

    bounds.removeFromTop(Dialog::margin);

    auto hexFileRow = bounds.removeFromTop(UILayout::textComponentHeight);
    hexFileLabel.setBounds(hexFileRow.removeFromLeft(columnWidth));
    hexFileSelector.setBounds(hexFileRow);

    warningLabel.setBounds(warningIcon.getRight(), warningIcon.getY(), (int) std::ceil(warningLabel.getTextWidth()), warningIcon.getHeight());
}

std::shared_ptr<ximu3::ConnectionConfig> UpdateFirmwareDialog::getConnectionConfig() const {
    if (deviceValue.getSelectedId() == 0) {
        return nullptr;
    }

    return ximu3::ConnectionConfig::from(devices[(size_t) deviceValue.getSelectedId() - 1]);
}

juce::File UpdateFirmwareDialog::getHexFile() const {
    return hexFileSelector.getFiles()[0];
}

void UpdateFirmwareDialog::launch(juce::ThreadPool &threadPool) {
    DialogQueue::getSingleton().pushFront(std::make_unique<UpdateFirmwareDialog>(), [&threadPool] {
        if (const auto *const updateFirmwareDialog = dynamic_cast<UpdateFirmwareDialog *>(DialogQueue::getSingleton().getActive())) {
            DialogQueue::getSingleton().pushFront(std::make_unique<UpdatingFirmwareDialog>(updateFirmwareDialog->getConnectionConfig(), updateFirmwareDialog->getHexFile(), threadPool));
        }
        return true;
    });
}
