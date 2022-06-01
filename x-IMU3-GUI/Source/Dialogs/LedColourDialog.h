#pragma once

#include "../Widgets/CustomTextEditor.h"
#include "../Widgets/IconButton.h"
#include "../Widgets/SimpleLabel.h"
#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <optional>

class DevicePanel;

class LedColourDialog : public Dialog,
                        private juce::ChangeListener
{
public:
    explicit LedColourDialog(DevicePanel& devicePanel_);

    ~LedColourDialog() override;

    void resized() override;

private:
    DevicePanel& devicePanel;

    IconButton defaultButton { IconButton::Style::menuStrip, BinaryData::default_svg, 0.8f, "Restore Normal LED Behaviour" };
    juce::ColourSelector colourSelector { juce::ColourSelector::showColourspace, 0, 0 };

    SimpleLabel redLabel { "Red:" };
    SimpleLabel greenLabel { "Green:" };
    SimpleLabel blueLabel { "Blue:" };
    SimpleLabel hexLabel { "Hex:" };

    CustomTextEditor redValue, greenValue, blueValue, hexValue;

    const juce::Colour nullColour = juce::Colour::fromRGB(0, 255, 255);

    bool inProgress = false;
    std::optional<juce::var> buffer;

    void sendColourCommand(const juce::var& value);

    void changeListenerCallback(juce::ChangeBroadcaster*) override;
};
