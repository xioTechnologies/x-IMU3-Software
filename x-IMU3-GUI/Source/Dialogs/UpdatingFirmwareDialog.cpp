#include "ApplicationSettings.h"
#include "CommandMessage.h"
#include "ErrorDialog.h"
#include "Firmware/Firmware.h"
#include "UpdateFirmwareDialog.h"
#include "UpdatingFirmwareDialog.h"
#include "Ximu3Bootloader.h"

UpdatingFirmwareDialog::UpdatingFirmwareDialog(std::unique_ptr<ximu3::ConnectionInfo> connectionInfo_, const juce::File& hexFile_)
        : Dialog(BinaryData::tools_svg, "Updating Firmware", "Cancel", ""),
          connectionInfo(std::move(connectionInfo_)),
          hexFile(hexFile_)
{
    addAndMakeVisible(progressBar);

    setOkButton(false);

    setSize(dialogWidth, calculateHeight(1));

    juce::Thread::launch([&]
                         {
                             const auto showError = [&](const juce::String& error, const bool tryAgain = true)
                             {
                                 juce::MessageManager::callAsync([&, error, tryAgain]
                                                                 {
                                                                     DialogQueue::getSingleton().pushFront(std::make_unique<ErrorDialog>(error + (tryAgain ? " Please try again." : "")));
                                                                     DialogQueue::getSingleton().pop();
                                                                     if (tryAgain)
                                                                     {
                                                                         UpdateFirmwareDialog::launch();
                                                                     }
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
                                 showError("Unable to open connection.");
                                 return;
                             }

                             // Read hardware version
                             const auto responses = connection->sendCommands({ "{\"hardwareVersion\":null}" }, ApplicationSettings::getSingleton().commands.retries, ApplicationSettings::getSingleton().commands.timeout);
                             if (responses.empty())
                             {
                                 showError("Unable to read hardware version.");
                                 return;
                             }
                             const auto hardwareVersion = CommandMessage(responses[0]).value.toString();

                             // Check compatibility
                             const auto firmwareIsV2 = hexFile.getFileName().startsWith("x-IMU3-Firmware-v2.");
                             const auto hardwareIsV2 = hardwareVersion.startsWith("v2.");
                             if (firmwareIsV2 != hardwareIsV2)
                             {
                                 showError("The detected " + hardwareVersion + " hardware is " + (firmwareIsV2 ? "not" : "only") + " compatible with v2.x.x firmware.", false);
                                 return;
                             }

                             // Send bootloader command
                             updateProgress("Sending Bootloader Command");
                             if (connection->sendCommands({ "{\"bootloader\":null}" }, ApplicationSettings::getSingleton().commands.retries, ApplicationSettings::getSingleton().commands.timeout).empty())
                             {
                                 showError("Unable to confirm bootloader command.");
                                 return;
                             }

                             // Close connection
                             connection->close();

                             // Upload
                             updateProgress("Waiting For Bootloader Mode");
                             juce::Thread::sleep(5000);

                             for (const auto& portName : ximu3::PortScanner::getPortNames())
                             {
                                 updateProgress("Attempting Upload on " + portName);

                                 if (XIMU3_upload_firmware(hardwareIsV2 ? "PIC32MZ2048EFG100" : "PIC32MZ2048EFG124", hexFile.getFullPathName().toRawUTF8(), portName.data()) == 0)
                                 {
                                     updateProgress("Update Complete", true);
                                     juce::Timer::callAfterDelay(1000, [&]
                                     {
                                         DialogQueue::getSingleton().pop();
                                     });
                                     return;
                                 }
                             }

                             showError("Firmware update failed.");
                         });
}

void UpdatingFirmwareDialog::resized()
{
    Dialog::resized();

    progressBar.setBounds(getContentBounds().removeFromTop(UILayout::textComponentHeight));
}
