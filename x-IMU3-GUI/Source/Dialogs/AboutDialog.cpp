#include "AboutDialog.h"

AboutDialog::AboutDialog(const juce::String& latestVersion) : Dialog(BinaryData::xio_icon_svg, "About", "Close", "", &sourceCodeButton, iconButtonWidth)
{
    addAndMakeVisible(logo);
    addAndMakeVisible(applicationNameLabel);
    addAndMakeVisible(applicationVersionLabel);
    addAndMakeVisible(expectedFirmwareVersionLabel);
    addAndMakeVisible(applicationNameValue);
    addAndMakeVisible(applicationVersionValue);
    addAndMakeVisible(expectedFirmwareVersionValue);
    addAndMakeVisible(sourceCodeButton);

    logo.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    logo.addMouseListener(this, true);

    addAndMakeVisible(applicationVersionUpdateLabel);
    applicationVersionUpdateLabel.setInterceptsMouseClicks(true, false);
    applicationVersionUpdateLabel.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    applicationVersionUpdateLabel.setTooltip(updateUrl);
    applicationVersionUpdateLabel.addMouseListener(this, true);
    applicationVersionUpdateLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    if (latestVersion.isNotEmpty())
    {
        applicationVersionUpdateLabel.setText(" (" + ((latestVersion == "v" + juce::JUCEApplication::getInstance()->getApplicationVersion()) ? "latest" : (latestVersion + " available")) + ")");
    }

    sourceCodeButton.onClick = [&]
    {
        juce::URL("https://github.com/xioTechnologies/x-IMU3-Software").launchInDefaultBrowser();
    };

    setSize(400, calculateHeight(5));
}

void AboutDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds();
    bounds.removeFromTop(10);
    logo.setBounds(bounds.removeFromTop(3 * UILayout::textComponentHeight));
    bounds.removeFromTop(margin);

    const auto rowHeight = bounds.getHeight() / 3;

    auto rowBounds = bounds.removeFromTop(rowHeight);
    applicationNameLabel.setBounds(rowBounds.removeFromLeft(200));
    applicationNameValue.setBounds(rowBounds);

    rowBounds = bounds.removeFromTop(rowHeight);
    applicationVersionLabel.setBounds(rowBounds.removeFromLeft(200));
    applicationVersionValue.setBounds(rowBounds.removeFromLeft((int) std::ceil(applicationVersionValue.getTextWidth())));
    if (applicationVersionUpdateLabel.isVisible())
    {
        applicationVersionUpdateLabel.setBounds(rowBounds.removeFromLeft((int) std::ceil(applicationVersionUpdateLabel.getTextWidth())));
    }

    rowBounds = bounds.removeFromTop(rowHeight);
    expectedFirmwareVersionLabel.setBounds(rowBounds.removeFromLeft(200));
    expectedFirmwareVersionValue.setBounds(rowBounds);
}

void AboutDialog::mouseDown(const juce::MouseEvent& mouseEvent)
{
    if (mouseEvent.eventComponent == &logo)
    {
        juce::URL(logoUrl).launchInDefaultBrowser();
    }
    else if (mouseEvent.eventComponent == &applicationVersionUpdateLabel)
    {
        juce::URL(updateUrl).launchInDefaultBrowser();
    }
}
