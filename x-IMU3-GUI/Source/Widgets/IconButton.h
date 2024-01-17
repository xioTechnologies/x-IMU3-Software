#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

class IconButton : public juce::DrawableButton
{
public:
    IconButton(const juce::String& icon, const juce::String& tooltip_,
               std::function<juce::PopupMenu()> getPopup_ = nullptr,
               const bool showPopupArrow = true,
               const juce::String& iconOn = {}, const juce::String& tooltipOn_ = {});

    void paint(juce::Graphics& g) override;

    juce::Rectangle<float> getImageBounds() const override;

    void resized() override;

    void clicked() override;

    using juce::Button::clicked;

    juce::String getTooltip() override;

    void setIcon(const juce::String& icon, const juce::String& iconOn);

private:
    const juce::String tooltip, tooltipOn;
    const std::function<juce::PopupMenu()> getPopup;
    const std::unique_ptr<juce::Drawable> popupArrow;

    juce::Rectangle<float> imageBounds;
    juce::Rectangle<float> popupArrowBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IconButton)
};
