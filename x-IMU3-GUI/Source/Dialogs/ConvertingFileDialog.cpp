#include <BinaryData.h>
#include "AreYouSureDialog.h"
#include "ConvertingFileDialog.h"
#include "ErrorDialog.h"

void ConvertingFileDialog::show(const juce::StringArray& sources_, const juce::String& destination_)
{
    const auto directory = juce::File(destination_).getChildFile(juce::File(sources_[0]).getFileNameWithoutExtension());

    if (directory.exists())
    {
        DialogQueue::getSingleton().pushBack(std::make_unique<DoYouWantToReplaceItDialog>(juce::File(sources_[0]).getFileName()), [directory, sources_, destination_]
        {
            directory.deleteRecursively();
            DialogQueue::getSingleton().pushBack(std::make_unique<ConvertingFileDialog>(sources_, destination_));
            return true;
        });
    }
    else
    {
        DialogQueue::getSingleton().pushBack(std::make_unique<ConvertingFileDialog>(sources_, destination_));
    }
}

ConvertingFileDialog::ConvertingFileDialog(const juce::StringArray& sources_, const juce::String& destination_)
        : Dialog(BinaryData::tools_svg, "Converting " + juce::File(sources_[0]).getFileName(), "Cancel", ""),
          sources(sources_),
          destination(destination_),
          file(juce::File(destination).getChildFile(juce::File(sources[0]).getFileNameWithoutExtension())),
          fileConverter(destination.toStdString(), sources[0].toStdString(), std::bind(&ConvertingFileDialog::progressCallback, this, std::placeholders::_1))
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
    if (sources.size() > 1)
    {
        sources.remove(0);
        show(sources, destination);
    }
    else
    {
        juce::File(destination).revealToUser();
    }

    DialogQueue::getSingleton().pop();
}
