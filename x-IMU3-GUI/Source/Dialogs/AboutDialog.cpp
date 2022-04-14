#include "AboutDialog.h"

AboutDialog::AboutDialog() : Dialog(BinaryData::xio_icon_svg, "About", "Close", "")
{
    addAndMakeVisible(applicationNameLabel);
    addAndMakeVisible(applicationVersionLabel);
    addAndMakeVisible(expectedFirmwareVersionLabel);
    addAndMakeVisible(sourceCodeLabel);
    addAndMakeVisible(applicationNameValue);
    addAndMakeVisible(applicationVersionValue);
    addAndMakeVisible(expectedFirmwareVersionValue);
    addAndMakeVisible(sourceCodeValue);
    addAndMakeVisible(logo);

    sourceCodeValue.setColour(juce::Label::textColourId, UIColours::hyperlink);
    sourceCodeValue.setInterceptsMouseClicks(true, false);
    sourceCodeValue.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    sourceCodeValue.setTooltip(sourceCodeUrl);
    sourceCodeValue.addMouseListener(this, true);

    logo.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    logo.addMouseListener(this, true);

    setSize(350, calculateHeight(6));
}

AboutDialog::~AboutDialog()
{
}

void AboutDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds();
    bounds.removeFromTop(10);
    logo.setBounds(bounds.removeFromTop(3 * UILayout::textComponentHeight));
    bounds.removeFromTop(margin);

    const auto rowHeight = bounds.getHeight() / 4;
    const auto position = [&](auto& label, auto& value)
    {
        auto rowBounds = bounds.removeFromTop(rowHeight);
        label.setBounds(rowBounds.removeFromLeft(200));
        value.setBounds(rowBounds);
    };
    position(applicationNameLabel, applicationNameValue);
    position(applicationVersionLabel, applicationVersionValue);
    position(expectedFirmwareVersionLabel, expectedFirmwareVersionValue);
    position(sourceCodeLabel, sourceCodeValue);
}

void AboutDialog::mouseDown(const juce::MouseEvent& mouseEvent)
{
    if (mouseEvent.eventComponent == &sourceCodeValue)
    {
        juce::URL(sourceCodeUrl).launchInDefaultBrowser();
    }
    else if (mouseEvent.eventComponent == &logo)
    {
        juce::URL(logoUrl).launchInDefaultBrowser();
    }
}
