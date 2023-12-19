#include "AreYouSureDialog.h"
#include <BinaryData.h>
#include "ConvertingFileDialog.h"
#include "ErrorDialog.h"

void ConvertingFileDialog::show(const std::vector<juce::File>& files_, const juce::File& destination_)
{
    const auto directory = destination_.getChildFile(files_[0].getFileNameWithoutExtension());

    if (directory.exists())
    {
        DialogQueue::getSingleton().pushBack(std::make_unique<DoYouWantToReplaceItDialog>(files_[0].getFileNameWithoutExtension()), [directory, files_, destination_]
        {
            directory.deleteRecursively();
            DialogQueue::getSingleton().pushBack(std::make_unique<ConvertingFileDialog>(files_, destination_));
            return true;
        });
    }
    else
    {
        DialogQueue::getSingleton().pushBack(std::make_unique<ConvertingFileDialog>(files_, destination_));
    }
}

ConvertingFileDialog::ConvertingFileDialog(const std::vector<juce::File>& files_, const juce::File& destination_)
        : Dialog(BinaryData::tools_svg, "Converting " + files_[0].getFileName(), "Cancel", ""),
          files(files_),
          destination(destination_),
          fileConverter(destination.getFullPathName().toStdString(), files[0].getFullPathName().toStdString(), std::bind(&ConvertingFileDialog::progressCallback, this, std::placeholders::_1))
{
    addAndMakeVisible(progressBar);

    setSize(dialogWidth, calculateHeight(1));
}

void ConvertingFileDialog::resized()
{
    Dialog::resized();

    progressBar.setBounds(getContentBounds().removeFromTop(UILayout::textComponentHeight));
}

void ConvertingFileDialog::progressCallback(ximu3::XIMU3_FileConverterProgress progress)
{
    juce::MessageManager::callAsync([&, self = SafePointer<juce::Component>(this), progress]
                                    {
                                        if (self == nullptr)
                                        {
                                            return;
                                        }

                                        progressBarValue = (double) (progress.percentage / 100.0f);
                                        progressBar.setTextToDisplay(juce::String(juce::roundToInt(progress.percentage)) + "% (" + juce::File::descriptionOfSizeInBytes((juce::int64) progress.bytes_processed) + " of " + juce::File::descriptionOfSizeInBytes((juce::int64) progress.file_size) + ")");

                                        switch (progress.status)
                                        {
                                            case ximu3::XIMU3_FileConverterStatusComplete:
                                                startTimer(1000);
                                                break;
                                            case ximu3::XIMU3_FileConverterStatusFailed:
                                                DialogQueue::getSingleton().pushFront(std::make_unique<ErrorDialog>("File converter failed."));
                                                DialogQueue::getSingleton().pop();
                                                break;
                                            case ximu3::XIMU3_FileConverterStatusInProgress:
                                                break;
                                        }
                                    });
}

void ConvertingFileDialog::timerCallback()
{
    if (files.size() > 1)
    {
        files.erase(files.begin());
        show(files, destination);
    }
    else
    {
        destination.revealToUser();
    }

    DialogQueue::getSingleton().pop();
}
