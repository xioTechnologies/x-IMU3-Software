#include "ApplicationSettings.h"
#include "Firmware/Firmware.h"
#include "UpdateFirmwareDialog.h"
#include "UpdatingFirmwareDialog.h"

UpdateFirmwareDialog::UpdateFirmwareDialog() : Dialog(BinaryData::tools_svg, "Update Firmware", "Update", "Cancel", &warningIcon, iconButtonWidth)
{
    addAndMakeVisible(deviceLabel);
    addAndMakeVisible(deviceValue);
    addAndMakeVisible(hexFileLabel);
    addAndMakeVisible(hexFileSelector);
    addAndMakeVisible(warningIcon);
    addAndMakeVisible(warningLabel);

    const auto hexFile = ApplicationSettings::getDirectory().getChildFile(Firmware::hexFile);
    hexFile.replaceWithData(Firmware::memoryBlock.getData(), Firmware::memoryBlock.getSize());
    hexFileSelector.setFiles({ hexFile });

    deviceValue.onChange = hexFileSelector.onChange = [&]
    {
        setOkButton(getConnectionInfo() != nullptr && hexFileSelector.isValid());
    };
    setOkButton(false);

    setSize(600, calculateHeight(2));
}

void UpdateFirmwareDialog::resized()
{
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

std::shared_ptr<ximu3::ConnectionInfo> UpdateFirmwareDialog::getConnectionInfo() const
{
    if (deviceValue.getSelectedId() == 0)
    {
        return nullptr;
    }

    return ximu3::connectionInfoFrom(devices[(size_t) deviceValue.getSelectedId() - 1]);
}

juce::File UpdateFirmwareDialog::getHexFile() const
{
    return hexFileSelector.getFiles()[0];
}

void UpdateFirmwareDialog::launch(juce::ThreadPool& threadPool)
{
    DialogQueue::getSingleton().pushFront(std::make_unique<UpdateFirmwareDialog>(), [&threadPool]
    {
        if (const auto* const updateFirmwareDialog = dynamic_cast<UpdateFirmwareDialog*>(DialogQueue::getSingleton().getActive()))
        {
            DialogQueue::getSingleton().pushFront(std::make_unique<UpdatingFirmwareDialog>(updateFirmwareDialog->getConnectionInfo(), updateFirmwareDialog->getHexFile(), threadPool));
        }
        return true;
    });
}
