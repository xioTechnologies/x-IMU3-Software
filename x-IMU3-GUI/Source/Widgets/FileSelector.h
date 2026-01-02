#pragma once

#include "CustomTextEditor.h"
#include "IconButton.h"

class FileSelector : public juce::Component, public juce::FileDragAndDropTarget {
public:
    FileSelector(const juce::String &tooltip, const std::optional<juce::String> &extension_);

    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray &) override;

    void filesDropped(const juce::StringArray &files, int, int) override;

    juce::Array<juce::File> getFiles() const;

    void setFiles(const juce::Array<juce::File> &files);

    void setFilesWhenEmpty(const juce::Array<juce::File> &files);

    bool isValid() const;

    bool isEmpty() const;

    std::function<void()> onChange;

private:
    const std::optional<juce::String> extension;

    CustomTextEditor textEditor;
    IconButton button;

    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::String getText() const;

    juce::StringArray toStringArray(const juce::Array<juce::File> &files) const;

    static bool exists(const juce::String &path);

    static juce::Array<juce::File> toFileArray(const juce::StringArray &strings);
};
