#include "ApplicationSettings.h"
#include "Firmware/Firmware.h"
#include "MessageDialog.h"
#include "UpdateFirmwareDialog.h"
#include "UpdatingFirmwareDialog.h"
#include "Ximu3Bootloader.h"

UpdatingFirmwareDialog::UpdatingFirmwareDialog(std::unique_ptr<ximu3::ConnectionConfig> config_, const juce::File &firmware_, juce::ThreadPool &threadPool)
    : Dialog(BinaryData::tools_svg, "Updating Firmware", "Cancel", ""),
      config(std::move(config_)),
      firmware(firmware_) {
    addAndMakeVisible(progressBar);

    setOkButton(false);

    setSize(dialogWidth, calculateHeight(1));

    threadPool.addJob([&, &threadPool_ = threadPool] {
        const auto showError = [&](const juce::String &error, const bool tryAgain = true) {
            juce::MessageManager::callAsync([&, error, tryAgain] {
                DialogQueue::getSingleton().pushFront(std::make_unique<ErrorDialog>(error, tryAgain ? "Try Again" : "OK"));
                DialogQueue::getSingleton().pop();
                if (tryAgain) {
                    UpdateFirmwareDialog::launch(threadPool_);
                }
            });
        };

        const auto updateProgress = [&](const auto &text, bool completed = false) {
            juce::MessageManager::callAsync([&, text, completed] {
                progressBarValue = completed ? 1.0 : (progressBarValue + (1.0 - progressBarValue) / 5);
                progressBar.setTextToDisplay(text);
            });
        };

        // Open connection
        updateProgress("Opening Connection");
        auto connection = std::make_unique<ximu3::Connection>(*config);
        if (connection->open() != ximu3::XIMU3_ResultOk) {
            showError("Unable to open connection");
            return;
        }

        const auto readSetting = [&](const std::string &key) {
            const auto response = connection->sendCommand("{\"" + key + "\":null}", ApplicationSettings::getSingleton().commands.retries, ApplicationSettings::getSingleton().commands.timeout);

            if (response.has_value() == false) {
                throw std::runtime_error("No response");
            }

            if (response->error.has_value()) {
                throw std::runtime_error(*response->error);
            }

            return response->value;
        };

        // Read hardware version
        std::string hardwareVersion;
        try {
            hardwareVersion = readSetting("hardware_version");
        } catch (const std::exception &e) {
            showError("Unable to read hardware version: " + juce::String(e.what()));
            return;
        }

        // Check compatibility
        const auto firmwareIsV2 = firmware.getFileName().startsWith("x-IMU3-Firmware-v2.");
        const auto hardwareIsV2 = hardwareVersion.starts_with("\"v2.");
        if (firmwareIsV2 != hardwareIsV2) {
            showError("The detected " + hardwareVersion + " hardware is " + (firmwareIsV2 ? "not" : "only") + " compatible with v2.x.x firmware", false);
            return;
        }

        // Send bootloader command
        updateProgress("Sending Bootloader Command");
        try {
            readSetting("bootloader");
        } catch (const std::exception &e) {
            showError("Unable to confirm bootloader command: " + juce::String(e.what()));
            return;
        }

        // Close connection
        connection->close();

        // Upload
        updateProgress("Waiting For Bootloader Mode");
        juce::Thread::sleep(5000);

        auto portNames = ximu3::PortScanner::getPortNames();
        std::stable_partition(portNames.begin(), portNames.end(), [](const auto &portName) {
            return juce::String(portName).containsIgnoreCase("usb");
        }); // reorder for USB ports first

        for (const auto &portName: portNames) {
            updateProgress("Attempting Upload on " + portName);

            if (XIMU3_upload_firmware(hardwareIsV2 ? "PIC32MZ2048EFG100" : "PIC32MZ2048EFG124", firmware.getFullPathName().toRawUTF8(), portName.c_str()) == 0) {
                updateProgress("Update Complete", true);
                juce::Timer::callAfterDelay(1000, [&] {
                    DialogQueue::getSingleton().pop();
                });
                return;
            }
        }

        showError("Firmware update failed");
    });
}

void UpdatingFirmwareDialog::resized() {
    Dialog::resized();

    progressBar.setBounds(getContentBounds().removeFromTop(UILayout::textComponentHeight));
}
