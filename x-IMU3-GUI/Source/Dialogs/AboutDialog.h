#pragma once

#include "Dialog.h"
#include "Firmware/Firmware.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/Icon.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"

class AboutDialog : public Dialog {
public:
    AboutDialog(const juce::String &latestVersion);

    void resized() override;

    void mouseUp(const juce::MouseEvent &mouseEvent) override;

private:
    const juce::String logoUrl{"https://x-io.co.uk"};

    Icon logo{BinaryData::xio_logo_svg, logoUrl};

    SimpleLabel versionLabel{"Version:"};
    SimpleLabel versionValue{"v" + juce::JUCEApplication::getInstance()->getApplicationVersion(), UIFonts::getDefaultFont(), juce::Justification::centredLeft};

    SimpleLabel buildDateLabel{"Build Date:"};
    SimpleLabel buildDateValue{juce::String(BUILD_DATE), UIFonts::getDefaultFont(), juce::Justification::centredLeft};

    SimpleLabel configurationLabel{"Configuration:"};
#if JUCE_DEBUG
    SimpleLabel configurationValue{"Debug", UIFonts::getDefaultFont(), juce::Justification::centredLeft};
#else
    SimpleLabel configurationValue{"Release", UIFonts::getDefaultFont(), juce::Justification::centredLeft};
#endif

    SimpleLabel commitShaLabel{"Commit SHA:"};
    SimpleLabel commitShaValue{juce::String(COMMIT_SHA).isEmpty() ? "N/A" : juce::String(COMMIT_SHA), UIFonts::getDefaultFont(), juce::Justification::centredLeft};

    const std::vector<std::pair<SimpleLabel *, SimpleLabel *> > rows
    {
        {&versionLabel, &versionValue},
        {&buildDateLabel, &buildDateValue},
        {&configurationLabel, &configurationValue},
        {&commitShaLabel, &commitShaValue},
    };

    SimpleLabel versionUpdateLabel{"", UIFonts::getDefaultFont(), juce::Justification::centredLeft};

    IconButton downloadsButton{BinaryData::download_svg, "x-IMU3 Downloads"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AboutDialog)
};
