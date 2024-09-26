#include "Dialogs/Dialog.h"
#include "FileSelector.h"
#include <filesystem>

FileSelector::FileSelector(const juce::String& tooltip, const std::optional<juce::String>& extension_) : extension(extension_), button(BinaryData::open_svg, tooltip)
{
    addAndMakeVisible(textEditor);
    addAndMakeVisible(button);
    textEditor.onTextChange = [&]
    {
        juce::NullCheckedInvocation::invoke(onChange);
    };
    button.onClick = [&]
    {
        const auto flags = [&]
        {
            if (extension)
            {
                return juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems;
            }
            return juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories;
        }();

        fileChooser = std::make_unique<juce::FileChooser>(button.getTooltip(), std::filesystem::exists(getText().toStdString()) ? textEditor.getText() : "", extension ? ("*" + *extension) : "");
        fileChooser->launchAsync(flags, [&] (const auto&)
        {
            if (fileChooser->getResults().isEmpty())
            {
                return;
            }

            setFiles(fileChooser->getResults());
        });
    };
}

void FileSelector::resized()
{
    auto r = getLocalBounds();
    button.setBounds(r.removeFromRight(Dialog::iconButtonWidth));
    textEditor.setBounds(r.withTrimmedRight(Dialog::margin));
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
    return toFileArray(juce::StringArray::fromTokens(getText(), ";", ""));
}

void FileSelector::setFiles(const juce::Array<juce::File>& files)
{
    textEditor.setText(toStringArray(files).joinIntoString(";"));
}

void FileSelector::setFilesWhenEmpty(const juce::Array<juce::File>& files)
{
    textEditor.setDefaultText(toStringArray(files).joinIntoString(";"));
}

bool FileSelector::isValid() const
{
    if (getText().isEmpty())
    {
        return false;
    }

    for (auto string : juce::StringArray::fromTokens(getText(), ";", ""))
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

bool FileSelector::isEmpty() const
{
    return textEditor.isEmpty();
}

juce::String FileSelector::getText() const
{
    return textEditor.getTextOrDefault();
}

juce::StringArray FileSelector::toStringArray(const juce::Array<juce::File>& files) const
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
    return strings;
}

juce::Array<juce::File> FileSelector::toFileArray(const juce::StringArray& strings)
{
    juce::Array<juce::File> files;
    for (auto file : strings)
    {
        if (std::filesystem::exists(file.toStdString()))
        {
            files.add(file);
        }
    }
    return files;
}
