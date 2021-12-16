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
