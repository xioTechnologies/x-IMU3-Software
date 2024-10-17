#include "AboutDialog.h"

AboutDialog::AboutDialog(const juce::String& latestVersion) : Dialog(BinaryData::xio_icon_svg, "About", "Close", "", &downloadsButton, iconButtonWidth)
{
    addAndMakeVisible(logo);
    for (auto& row : rows)
    {
        addAndMakeVisible(row.first);
        addAndMakeVisible(row.second);
    }
    addAndMakeVisible(downloadsButton);

    logo.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    logo.addMouseListener(this, true);

    if (latestVersion.isNotEmpty())
    {
        addAndMakeVisible(versionUpdateLabel);
        if (latestVersion != ("v" + juce::JUCEApplication::getInstance()->getApplicationVersion()))
        {
            versionUpdateLabel.setInterceptsMouseClicks(true, true);
            versionUpdateLabel.addMouseListener(this, true);
            versionUpdateLabel.setMouseCursor(juce::MouseCursor::PointingHandCursor);
            versionUpdateLabel.setText(latestVersion + " available");
            versionUpdateLabel.setColour(juce::Label::textColourId, UIColours::update);
        }
        else
        {
            versionUpdateLabel.setText("No updates available");
            versionUpdateLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
        }
    }

    downloadsButton.onClick = [&]
    {
        juce::URL("https://x-io.co.uk/x-imu3/#downloads").launchInDefaultBrowser();
    };

    setSize(375, 325);
}

void AboutDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds();

    bounds.removeFromTop(margin);
    logo.setBounds(bounds.removeFromTop(100));
    bounds.removeFromTop(margin);

    const auto labelWidth = std::reduce(rows.begin(), rows.end(), 0, [&] (int width, const auto& pair) { return std::max(width, juce::GlyphArrangement::getStringWidthInt(UIFonts::getDefaultFont(), pair.first->getText())); });
    const auto valueWidth = std::reduce(rows.begin(), rows.end(), 0, [&] (int width, const auto& pair) { return std::max(width, juce::GlyphArrangement::getStringWidthInt(UIFonts::getDefaultFont(), pair.second->getText())); });
    bounds = bounds.withSizeKeepingCentre(labelWidth + 25 + valueWidth, bounds.getHeight());
    for (auto it = rows.rbegin(); it != rows.rend(); it++)
    {
        auto row = bounds.removeFromBottom(UILayout::textComponentHeight);
        it->first->setBounds(row.removeFromLeft(labelWidth));
        it->second->setBounds(row.removeFromRight(valueWidth));
    }

    versionUpdateLabel.setBounds(downloadsButton.getRight(), downloadsButton.getY(), (int) std::ceil(versionUpdateLabel.getTextWidth()), downloadsButton.getHeight());
}

void AboutDialog::mouseUp(const juce::MouseEvent& mouseEvent)
{
    if (mouseEvent.eventComponent == &logo)
    {
        juce::URL(logoUrl).launchInDefaultBrowser();
    }
    else if (mouseEvent.eventComponent == &versionUpdateLabel)
    {
        downloadsButton.triggerClick();
    }
}
