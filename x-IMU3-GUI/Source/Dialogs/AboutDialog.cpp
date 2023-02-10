#include "AboutDialog.h"

AboutDialog::AboutDialog() : Dialog(BinaryData::xio_icon_svg, "About", "Close", "")
{
    addAndMakeVisible(logo);
    addAndMakeVisible(applicationNameLabel);
    addAndMakeVisible(applicationVersionLabel);
    addAndMakeVisible(expectedFirmwareVersionLabel);
    addAndMakeVisible(sourceCodeLabel);
    addAndMakeVisible(applicationNameValue);
    addAndMakeVisible(applicationVersionValue);
    addChildComponent(applicationVersionUpdateLabel);
    addAndMakeVisible(expectedFirmwareVersionValue);
    addAndMakeVisible(sourceCodeValue);

    logo.setMouseCursor(juce::MouseCursor::PointingHandCursor);
    logo.addMouseListener(this, true);

    const auto initialiseUrl = [&](auto& label, const juce::String& url)
    {
        label.setColour(juce::Label::textColourId, UIColours::hyperlink);
        label.setInterceptsMouseClicks(true, false);
        label.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        label.setTooltip(url);
        label.addMouseListener(this, true);
    };

    initialiseUrl(applicationVersionUpdateLabel, updateUrl);
    initialiseUrl(sourceCodeValue, sourceCodeUrl);

    juce::Thread::launch([&, self = SafePointer<juce::Component>(this)]
                         {
                             const auto parsed = juce::JSON::parse(juce::URL("https://api.github.com/repos/xioTechnologies/x-IMU3-Software/releases/latest").readEntireTextStream());

                             juce::MessageManager::callAsync([&, self, parsed]
                                                             {
                                                                 if (self == nullptr)
                                                                 {
                                                                     return;
                                                                 }

                                                                 if (const auto* const object = parsed.getDynamicObject())
                                                                 {
                                                                     const auto tagName = object->getProperty("tag_name").toString();

                                                                     if (applicationVersionValue.getText() != tagName)
                                                                     {
                                                                         applicationVersionUpdateLabel.setVisible(true);
                                                                         applicationVersionUpdateLabel.setText(" (" + tagName + " available)");
                                                                     }

                                                                     resized();
                                                                 }
                                                             });
                         });

    setSize(400, calculateHeight(6));
}

void AboutDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds();
    bounds.removeFromTop(10);
    logo.setBounds(bounds.removeFromTop(3 * UILayout::textComponentHeight));
    bounds.removeFromTop(margin);

    const auto rowHeight = bounds.getHeight() / 4;

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

    rowBounds = bounds.removeFromTop(rowHeight);
    sourceCodeLabel.setBounds(rowBounds.removeFromLeft(200));
    sourceCodeValue.setBounds(rowBounds);
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
    else if (mouseEvent.eventComponent == &sourceCodeValue)
    {
        juce::URL(sourceCodeUrl).launchInDefaultBrowser();
    }
}
