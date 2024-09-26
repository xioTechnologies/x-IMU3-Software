#include <BinaryData.h>
#include "ConvertingFilesDialog.h"
#include "MessageDialog.h"

ConvertingFilesDialog::ConvertingFilesDialog(const juce::Array<juce::File>& files, const juce::File& destination_, const juce::String& name_)
        : Dialog(BinaryData::tools_svg, "Converting Files", "Cancel", ""),
          destination(destination_),
          name(name_),
          fileConverter(juce::File::addTrailingSeparator (destination.getFullPathName()).toStdString(), name.toStdString(), [&]
          {
              std::vector<std::string> stringVector;
              for (const auto& file : files)
              {
                  stringVector.push_back(file.getFullPathName().toStdString());
              }
              return stringVector;
          }(), std::bind(&ConvertingFilesDialog::progressCallback, this, std::placeholders::_1))
{
    addAndMakeVisible(progressBar);

    setSize(dialogWidth, calculateHeight(1));
}

void ConvertingFilesDialog::resized()
{
    Dialog::resized();

    progressBar.setBounds(getContentBounds().removeFromTop(UILayout::textComponentHeight));
}

void ConvertingFilesDialog::progressCallback(ximu3::XIMU3_FileConverterProgress progress)
{
    juce::MessageManager::callAsync([&, self = SafePointer<juce::Component>(this), progress]
                                    {
                                        if (self == nullptr)
                                        {
                                            return;
                                        }

                                        progressBarValue = (double) (progress.percentage / 100.0f);
                                        progressBar.setTextToDisplay(juce::String(juce::roundToInt(progress.percentage)) + "% (" + juce::File::descriptionOfSizeInBytes((juce::int64) progress.bytes_processed) + " of " + juce::File::descriptionOfSizeInBytes((juce::int64) progress.bytes_total) + ")");

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

void ConvertingFilesDialog::timerCallback()
{
    destination.getChildFile(name).startAsProcess();
    DialogQueue::getSingleton().pop();
}
