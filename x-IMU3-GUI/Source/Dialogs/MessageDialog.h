#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>

class MessageDialog : public Dialog {
public:
    MessageDialog(const juce::String &icon_,
                  const juce::String &dialogTitle,
                  const juce::String &message,
                  const juce::String &okButtonText,
                  const juce::String &cancelButtonText);

    void paint(juce::Graphics &g) override;

private:
    juce::TextLayout textLayout;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageDialog)
};

class ConfirmDialog : public MessageDialog {
public:
    explicit ConfirmDialog(const juce::String &text);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfirmDialog)
};

class ConfirmReplaceDialog : public ConfirmDialog {
public:
    explicit ConfirmReplaceDialog(const juce::String &name);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfirmReplaceDialog)
};

class ErrorDialog : public MessageDialog {
public:
    explicit ErrorDialog(const juce::String &error);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ErrorDialog)
};
