#include "../ApplicationSettings.h"
#include "../Firmware/Firmware.h"
#include "ErrorDialog.h"
#include "UpdatingFirmwareDialog.h"
#include "Ximu3Bootloader.h"

UpdatingFirmwareDialog::UpdatingFirmwareDialog(std::unique_ptr<ximu3::ConnectionInfo> connectionInfo_, const juce::String& fileName_)
        : Dialog(BinaryData::tools_svg, "Updating Firmware", "Cancel", ""),
          connectionInfo(std::move(connectionInfo_)),
          fileName(fileName_)
{
    addAndMakeVisible(progressBar);

    setOkButton(false);

    setSize(dialogWidth, calculateHeight(1));

    juce::Thread::launch([&]
                         {
                             const auto showError = [&]
                             {
                                 juce::MessageManager::callAsync([&]
                                                                 {
                                                                     DialogQueue::getSingleton().pushFront(std::make_unique<ErrorDialog>("Firmware update failed."));
                                                                     DialogQueue::getSingleton().pop();
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
                             if (connection->sendCommands({ "{\"bootloader\":null}" }, ApplicationSettings::getSingleton().commands.retries, ApplicationSettings::getSingleton().commands.timeout).empty())
                             {
                                 showError();
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

                                 if (XIMU3_upload_firmware("PIC32MZ2048EFG124", fileName.toRawUTF8(), portName.data()) == 0)
                                 {
                                     updateProgress("Update Complete", true);
                                     juce::Timer::callAfterDelay(1000, [&]
                                     {
                                         DialogQueue::getSingleton().pop();
                                     });
                                     return;
                                 }
                             }

                             showError();
                         });
}

void UpdatingFirmwareDialog::resized()
{
    Dialog::resized();

    progressBar.setBounds(getContentBounds().removeFromTop(UILayout::textComponentHeight));
}
