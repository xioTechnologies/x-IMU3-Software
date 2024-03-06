#include <BinaryData.h>
#include "MessageDialog.h"

MessageDialog::MessageDialog(const juce::String& icon_, const juce::String& dialogTitle, const juce::String& message, const juce::String& okButtonText, const juce::String& cancelButtonText) : Dialog(icon_, dialogTitle, okButtonText, cancelButtonText)
{
    juce::AttributedString string;
    string.append(message, UIFonts::getDefaultFont(), UIColours::foreground);
    string.setLineSpacing(3.0f);
    textLayout.createLayout(string, dialogWidth - margin);

    setSize(dialogWidth, calculateHeight(0) + textLayout.getHeight());
}

void MessageDialog::paint(juce::Graphics& g)
{
    textLayout.draw(g, getContentBounds().toFloat());
}

AreYouSureDialog::AreYouSureDialog(const juce::String& text) : MessageDialog(BinaryData::warning_white_svg, "Are You Sure?", text, "Yes", "Cancel")
{
}

DoYouWantToReplaceItDialog::DoYouWantToReplaceItDialog(const juce::String& name) : AreYouSureDialog(name + " already exists. Do you want to replace it?")
{
}

ErrorDialog::ErrorDialog(const juce::String& error) : MessageDialog(BinaryData::warning_white_svg, "Error", error, "OK", "")
{
}
