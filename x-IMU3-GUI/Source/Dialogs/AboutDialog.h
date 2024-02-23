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

    void mouseUp(const juce::MouseEvent& mouseEvent) override;

private:
    const juce::String logoUrl { "https://x-io.co.uk" };

    Icon logo { BinaryData::xio_logo_svg, logoUrl };

    SimpleLabel applicationVersionLabel { "x-IMU3 GUI Version:" };
    SimpleLabel applicationVersionValue { "v" + juce::JUCEApplication::getInstance()->getApplicationVersion(), UIFonts::getDefaultFont(), juce::Justification::centredRight };
    SimpleLabel expectedFirmwareVersionLabel { "Expected Firmware:" };
    SimpleLabel expectedFirmwareVersionValue { Firmware::version, UIFonts::getDefaultFont(), juce::Justification::centredRight };

    SimpleLabel applicationVersionUpdateLabel { "", UIFonts::getDefaultFont(), juce::Justification::centredLeft };

    IconButton downloadsButton { BinaryData::download_svg, "x-IMU3 Downloads" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AboutDialog)
};
