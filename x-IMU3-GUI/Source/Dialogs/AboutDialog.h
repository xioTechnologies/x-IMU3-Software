#pragma once

#include "Dialog.h"
#include "Firmware/Firmware.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/Icon.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"

class AboutDialog : public Dialog
{
public:
    AboutDialog(const juce::String& latestVersion);

    void resized() override;

    void mouseDown(const juce::MouseEvent& mouseEvent) override;

private:
    const juce::String logoUrl { "https://x-io.co.uk" };
    const juce::String updateUrl { "https://x-io.co.uk/x-imu3/#downloads" };

    Icon logo { BinaryData::xio_logo_svg, logoUrl };

    SimpleLabel applicationNameLabel { "Application Name:" };
    SimpleLabel applicationVersionLabel { "Application Version:" };
    SimpleLabel expectedFirmwareVersionLabel { "Expected Firmware Version:" };
    SimpleLabel applicationNameValue { juce::JUCEApplication::getInstance()->getApplicationName() };
    SimpleLabel applicationVersionValue { "v" + juce::JUCEApplication::getInstance()->getApplicationVersion() };
    SimpleLabel applicationVersionUpdateLabel { "", UIFonts::getDefaultFont(), juce::Justification::centredRight };
    SimpleLabel expectedFirmwareVersionValue { Firmware::version };

    IconButton sourceCodeButton { BinaryData::github_svg, "Source Code" };
};
