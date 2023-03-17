#pragma once

#include "../Firmware/Firmware.h"
#include "../Widgets/Icon.h"
#include "../Widgets/SimpleLabel.h"
#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>

class AboutDialog : public Dialog
{
public:
    AboutDialog();

    void resized() override;

    void mouseDown(const juce::MouseEvent& mouseEvent) override;

private:
    const juce::String logoUrl { "https://x-io.co.uk" };
    const juce::String updateUrl { "https://x-io.co.uk/x-imu3/#downloads" };
    const juce::String sourceCodeUrl { "https://github.com/xioTechnologies/x-IMU3-Software" };

    Icon logo { BinaryData::xio_logo_svg, logoUrl };

    SimpleLabel applicationNameLabel { "Application Name:" };
    SimpleLabel applicationVersionLabel { "Application Version:" };
    SimpleLabel expectedFirmwareVersionLabel { "Expected Firmware Version:" };
    SimpleLabel sourceCodeLabel { "Source Code:" };
    SimpleLabel applicationNameValue { juce::JUCEApplication::getInstance()->getApplicationName() };
    SimpleLabel applicationVersionValue { "v" + juce::JUCEApplication::getInstance()->getApplicationVersion() };
    SimpleLabel applicationVersionUpdateLabel { "", UIFonts::getDefaultFont(), juce::Justification::centredRight };
    SimpleLabel expectedFirmwareVersionValue { Firmware::version };
    SimpleLabel sourceCodeValue { "GitHub" };
};
