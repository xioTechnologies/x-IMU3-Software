#include "ErrorDialog.h"
#include "FileConverterProgressDialog.h"

FileConverterProgressDialog::FileConverterProgressDialog(const juce::String& source, const juce::String& destination)
        : Dialog(BinaryData::tools_svg, "File Converter Progress", "Cancel", ""),
          file(juce::File(destination).getChildFile(juce::File(source).getFileNameWithoutExtension())),
          fileConverter(destination.toStdString(), source.toStdString(), std::bind(&FileConverterProgressDialog::progressCallback, this, std::placeholders::_1))
{
    addAndMakeVisible(progressBar);

    progressBar.setColour(juce::ProgressBar::backgroundColourId, UIColours::menuStripButton);
    progressBar.setColour(juce::ProgressBar::foregroundColourId, UIColours::background);

    setSize(dialogWidth, calculateHeight(1));
}

void FileConverterProgressDialog::resized()
{
    Dialog::resized();

    progressBar.setBounds(getContentBounds().removeFromTop(UILayout::textComponentHeight));
}

void FileConverterProgressDialog::progressCallback(ximu3::XIMU3_FileConverterProgress progress)
{
    juce::MessageManager::callAsync([&, self = SafePointer<juce::Component>(this), progress]
                                    {
                                        if (self == nullptr)
                                        {
                                            return;
                                        }

                                        progressBarValue = (double) (progress.percentage / 100.0f);
                                        progressBar.setTextToDisplay(juce::String(juce::roundToInt(progress.percentage)) + "% (" + juce::String((float) progress.bytes_processed / 1E6f, 1) + " of " + juce::String((float) progress.file_size / 1E6f, 1) + " MB)");

                                        switch (progress.status)
                                        {
                                            case ximu3::XIMU3_FileConverterStatusComplete:
                                                startTimer(1000);
                                                break;
                                            case ximu3::XIMU3_FileConverterStatusFailed:
                                                DialogLauncher::launchDialog(std::make_unique<ErrorDialog>("File converter failed."));
                                                break;
                                            case ximu3::XIMU3_FileConverterStatusInProgress:
                                                break;
                                        }
                                    });
}

void FileConverterProgressDialog::timerCallback()
{
    file.revealToUser();
    DialogLauncher::launchDialog(nullptr);
}
