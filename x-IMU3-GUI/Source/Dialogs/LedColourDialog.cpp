#include "../DevicePanel/DevicePanel.h"
#include <algorithm>
#include "LedColourDialog.h"
#include <limits>

LedColourDialog::LedColourDialog(DevicePanel& devicePanel_)
        : Dialog(BinaryData::json_svg, "LED Colour", "Close", "", &defaultButton, 60),
          devicePanel(devicePanel_)
{
    addAndMakeVisible(redLabel);
    addAndMakeVisible(greenLabel);
    addAndMakeVisible(blueLabel);
    addAndMakeVisible(hexLabel);
    addAndMakeVisible(redValue);
    addAndMakeVisible(greenValue);
    addAndMakeVisible(blueValue);
    addAndMakeVisible(hexValue);
    addAndMakeVisible(colourSelector);
    addAndMakeVisible(defaultButton);

    redValue.onTextChange = blueValue.onTextChange = greenValue.onTextChange = [this]
    {
        const auto min = (int) std::numeric_limits<juce::uint8>::min();
        const auto max = (int) std::numeric_limits<juce::uint8>::max();

        const auto red = (juce::uint8) std::clamp(redValue.getText().getIntValue(), min, max);
        const auto green = (juce::uint8) std::clamp(greenValue.getText().getIntValue(), min, max);
        const auto blue = (juce::uint8) std::clamp(blueValue.getText().getIntValue(), min, max);

        colourSelector.setCurrentColour(juce::Colour::fromRGB(red, green, blue));
    };

    hexValue.onTextChange = [this]
    {
        colourSelector.setCurrentColour(juce::Colour::fromString("FF" + hexValue.getText()));
    };

    defaultButton.onClick = [&]
    {
        redValue.setText("", false);
        greenValue.setText("", false);
        blueValue.setText("", false);
        hexValue.setText("", false);
        colourSelector.setCurrentColour(nullColour, juce::dontSendNotification);

        devicePanel.sendCommands({{ "colour", {}}}); // TODO: Indicate failed command to user
    };

    colourSelector.setCurrentColour(nullColour, juce::dontSendNotification);
    colourSelector.setColour(juce::ColourSelector::backgroundColourId, {});
    colourSelector.addChangeListener(this);
    colourSelector.setWantsKeyboardFocus(true);

    setSize(425, calculateHeight(4));
}

LedColourDialog::~LedColourDialog()
{
}

void LedColourDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds();

    colourSelector.setBounds(bounds.removeFromRight(juce::roundToInt((bounds.getHeight() + 4) / 0.85f)));  // square, based on juce::ColourSelector::resized

    bounds.removeFromRight(juce::roundToInt(Dialog::margin * 2.5f));
    const auto setRow = [&](auto& label, auto& value)
    {
        auto row = bounds.removeFromTop(UILayout::textComponentHeight);
        label.setBounds(row.removeFromLeft(70));
        value.setBounds(row);
        bounds.removeFromTop(Dialog::margin);
    };
    setRow(redLabel, redValue);
    setRow(greenLabel, greenValue);
    setRow(blueLabel, blueValue);
    setRow(hexLabel, hexValue);
}

void LedColourDialog::changeListenerCallback(juce::ChangeBroadcaster*)
{
    const auto setText = [](auto& value, const auto& text)
    {
        if (value.hasKeyboardFocus(true) == false)
        {
            value.setText(text, false);
        }
    };

    const auto colour = colourSelector.getCurrentColour();
    setText(redValue, juce::String(colour.getRed()));
    setText(greenValue, juce::String(colour.getGreen()));
    setText(blueValue, juce::String(colour.getBlue()));

    const auto colourText = colour.toDisplayString(false);
    setText(hexValue, colourText);

    devicePanel.sendCommands({ CommandMessage("colour", colourText) }); // TODO: Indicate failed command to user
}
