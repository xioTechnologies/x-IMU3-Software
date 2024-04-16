#include "FileSelector.h"
#include <filesystem>
#include "Dialogs/Dialog.h"

FileSelector::FileSelector(const juce::String& tooltip, const std::optional<juce::String>& extension_) : extension(extension_), button(BinaryData::open_svg, tooltip)
{
    addAndMakeVisible(value);
    addAndMakeVisible(button);
    value.onTextChange = [&]
    {
        juce::NullCheckedInvocation::invoke(onChange);
    };
    button.onClick = [&]
    {
        juce::FileChooser fileChooser(button.getTooltip(), std::filesystem::exists(value.getText().toStdString()) ? value.getText() : "", extension ? ("*" + *extension) : "");
        if (fileChooser.browseForMultipleFilesOrDirectories())
        {
            setFiles(fileChooser.getResults());
        }
    };
}

void FileSelector::resized()
{
    auto r = getLocalBounds();
    button.setBounds(r.removeFromRight(Dialog::iconButtonWidth));
    value.setBounds(r.withTrimmedRight(Dialog::margin));
}

bool FileSelector::isInterestedInFileDrag(const juce::StringArray&)
{
    return true;
}

void FileSelector::filesDropped(const juce::StringArray& files, int, int)
{
    setFiles(toFileArray(files));
}

juce::Array<juce::File> FileSelector::getFiles() const
{
    return toFileArray(juce::StringArray::fromTokens(value.getText(), ";", ""));
}

void FileSelector::setFiles(const juce::Array<juce::File>& files)
{
    juce::StringArray strings;
    for (auto file : files)
    {
        if (extension.has_value() == false && file.isDirectory() == false)
        {
            file = file.getParentDirectory();
        }

        strings.add(file.getFullPathName());
    }
    value.setText(strings.joinIntoString(";"));
}

bool FileSelector::isValid() const
{
    if (value.getText().isEmpty())
    {
        return false;
    }

    for (auto string : juce::StringArray::fromTokens(value.getText(), ";", ""))
    {
        if (std::filesystem::exists(string.toStdString()) == false)
        {
            return false;
        }
        if (extension && std::filesystem::path(string.toStdString()).extension() != extension->toStdString())
        {
            return false;
        }
    }
    return true;
}

juce::Array<juce::File> FileSelector::toFileArray(const juce::StringArray& strings)
{
    juce::Array<juce::File> files;
    for (auto file : strings)
    {
        files.add(file);
    }
    return files;
}
