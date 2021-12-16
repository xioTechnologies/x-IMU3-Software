#pragma once

#include "../Widgets/Icon.h"
#include "../Widgets/SimpleLabel.h"
#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>

class AboutDialog : public Dialog
{
public:
    AboutDialog();

    ~AboutDialog() override;

    void resized() override;

private:
    const juce::String logoUrl { "https://x-io.co.uk" };
    const juce::String sourceCodeUrl { "https://github.com/xioTechnologies/x-IMU3-Software" };

    SimpleLabel applicationNameLabel { "Application Name:" };
    SimpleLabel applicationVersionLabel { "Application Version:" };
    SimpleLabel expectedFirmwareVersionLabel { "Expected Firmware Version:" };
    SimpleLabel sourceCodeLabel { "Source Code:" };
    SimpleLabel applicationNameValue { juce::JUCEApplication::getInstance()->getApplicationName() };
    SimpleLabel applicationVersionValue { "v" + juce::JUCEApplication::getInstance()->getApplicationVersion() };
    SimpleLabel expectedFirmwareVersionValue { "vX.X" }; // TODO: Get version from bootloader feature or similar
    SimpleLabel sourceCodeValue { "github", UIFonts::defaultFont, juce::Justification::centredLeft, sourceCodeUrl };

    Icon logo { BinaryData::xio_logo_svg, logoUrl, logoUrl };
};
