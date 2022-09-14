#include "../ApplicationSettings.h"
#include "../Firmware/Firmware.h"
#include "ErrorDialog.h"
#include "UpdatingFirmwareDialog.h"
#include "Ximu3Bootloader.h"

UpdatingFirmwareDialog::UpdatingFirmwareDialog(std::unique_ptr<ximu3::ConnectionInfo> connectionInfo_, const juce::String& fileName_)
        : Dialog(BinaryData::tools_svg, "Updating Firmware", "Cancel", ""),
          juce::Thread("Updating Firmware"),
          connectionInfo(std::move(connectionInfo_)),
          fileName(fileName_)
{
    addAndMakeVisible(progressBar);

    setValid(false);

    setSize(dialogWidth, calculateHeight(1));

    startThread();
}

UpdatingFirmwareDialog::~UpdatingFirmwareDialog()
{
    stopThread(10000);
}

void UpdatingFirmwareDialog::resized()
{
    Dialog::resized();

    progressBar.setBounds(getContentBounds().removeFromTop(UILayout::textComponentHeight));
}

void UpdatingFirmwareDialog::run()
{
    const auto showError = [&]
    {
        juce::MessageManager::callAsync([&]
                                        {
                                            DialogLauncher::launchDialog(std::make_unique<ErrorDialog>("Firmware update failed."));
                                        });
    };

    const auto updateProgress = [&](const auto& text, bool completed = false)
    {
        juce::MessageManager::callAsync([&, text, completed]
                                        {
                                            progressBarValue = completed ? 1.0 : (progressBarValue + (1.0 - progressBarValue) / 5);
                                            progressBar.setTextToDisplay(text);
                                        });
    };

    // Open connection
    updateProgress("Opening Connection");
    auto connection = std::make_unique<ximu3::Connection>(*connectionInfo);
    if (connection->open() != ximu3::XIMU3_ResultOk)
    {
        showError();
        return;
    }

    // Send bootloader command
    updateProgress("Sending Bootloader Command");
    if (connection->sendCommands({ "{\"bootloader\":null}" }, ApplicationSettings::getSingleton().retries, ApplicationSettings::getSingleton().timeout).empty())
    {
        showError();
        return;
    }

    // Close connection
    connection->close();

    // Upload
    updateProgress("Waiting For Bootloader Mode");
    juce::Thread::sleep(5000);

    for (const auto& port : ximu3::SerialDiscovery::getAvailablePorts())
    {
        updateProgress("Attempting Upload on " + port);

        if (XIMU3_upload_firmware("PIC32MZ2048EFG124", fileName.toRawUTF8(), port.data()) == 0)
        {
            updateProgress("Update Complete", true);
            juce::Timer::callAfterDelay(1000, [&]
            {
                DialogLauncher::launchDialog(nullptr);
            });
            return;
        }
    }

    showError();
}
