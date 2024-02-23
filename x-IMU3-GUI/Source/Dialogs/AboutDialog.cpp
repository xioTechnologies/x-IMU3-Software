#include "AboutDialog.h"

AboutDialog::AboutDialog(const juce::String& latestVersion) : Dialog(BinaryData::xio_icon_svg, "About", "Close", "", &downloadsButton, iconButtonWidth)
{
    addAndMakeVisible(logo);
    addAndMakeVisible(applicationVersionLabel);
    addAndMakeVisible(applicationVersionValue);
    addAndMakeVisible(expectedFirmwareVersionLabel);
    addAndMakeVisible(expectedFirmwareVersionValue);
    addAndMakeVisible(downloadsButton);

    logo.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    logo.addMouseListener(this, true);

    if (latestVersion.isNotEmpty())
    {
        addAndMakeVisible(applicationVersionUpdateLabel);
        if (latestVersion != ("v" + juce::JUCEApplication::getInstance()->getApplicationVersion()))
        {
            applicationVersionUpdateLabel.setInterceptsMouseClicks(true, true);
            applicationVersionUpdateLabel.addMouseListener(this, true);
            applicationVersionUpdateLabel.setMouseCursor(juce::MouseCursor::PointingHandCursor);
            applicationVersionUpdateLabel.setText("x-IMU3 GUI " + latestVersion + " available");
            applicationVersionUpdateLabel.setColour(juce::Label::textColourId, UIColours::update);
        }
        else
        {
            applicationVersionUpdateLabel.setText("No updates available");
            applicationVersionUpdateLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
        }
    }

    downloadsButton.onClick = [&]
    {
        juce::URL("https://x-io.co.uk/x-imu3/#downloads").launchInDefaultBrowser();
    };

    setSize(375, 235);
}

void AboutDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds();

    expectedFirmwareVersionLabel.setBounds(bounds.removeFromBottom(UILayout::textComponentHeight).reduced(75, 0));
    expectedFirmwareVersionValue.setBounds(expectedFirmwareVersionLabel.getBounds());
    applicationVersionLabel.setBounds(bounds.removeFromBottom(UILayout::textComponentHeight).reduced(75, 0));
    applicationVersionValue.setBounds(applicationVersionLabel.getBounds());

    bounds.removeFromBottom(margin);
    bounds.removeFromTop(10);
    logo.setBounds(bounds);
    bounds.removeFromTop(margin);

    applicationVersionUpdateLabel.setBounds(downloadsButton.getRight(), downloadsButton.getY(), 200, downloadsButton.getHeight());
}

void AboutDialog::mouseUp(const juce::MouseEvent& mouseEvent)
{
    if (mouseEvent.eventComponent == &logo)
    {
        juce::URL(logoUrl).launchInDefaultBrowser();
    }
    else if (mouseEvent.eventComponent == &applicationVersionUpdateLabel)
    {
        downloadsButton.triggerClick();
    }
}
