#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>

class MessageDialog : public Dialog
{
public:
    MessageDialog(const juce::String& icon_,
                  const juce::String& dialogTitle,
                  const juce::String& message,
                  const juce::String& okButtonText,
                  const juce::String& cancelButtonText);

    void paint(juce::Graphics& g) override;

private:
    juce::TextLayout textLayout;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageDialog)
};

class AreYouSureDialog : public MessageDialog
{
public:
    explicit AreYouSureDialog(const juce::String& text = "Are you sure?");

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AreYouSureDialog)
};

class DoYouWantToReplaceItDialog : public AreYouSureDialog
{
public:
    explicit DoYouWantToReplaceItDialog(const juce::String& name);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DoYouWantToReplaceItDialog)
};

class ErrorDialog : public MessageDialog
{
public:
    explicit ErrorDialog(const juce::String& error);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ErrorDialog)
};

