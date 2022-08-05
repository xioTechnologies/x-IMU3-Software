#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

class IconButton : public juce::DrawableButton
{
public:
    enum class Style
    {
        normal,
        menuStrip,
        menuStripDropdown,
    };

    IconButton(const Style style_,
               const juce::String& icon, const float scale_, const juce::String& tooltip_,
               std::function<juce::PopupMenu()> getPopup_ = nullptr,
               const juce::String& iconOn = {}, const float scaleOn_ = 1.0f, const juce::String& tooltipOn_ = {});

    juce::Rectangle<float> getImageBounds() const override;

    void buttonStateChanged() override;

    void resized() override;

    void enablementChanged() override;

    void clicked() override;

    using juce::Button::clicked;

    juce::String getTooltip() override;

    void setIcon(const juce::String& icon, const juce::String& iconOn);

private:
    const Style style;
    const float scale, scaleOn;
    const juce::String tooltip, tooltipOn;
    const std::function<juce::PopupMenu()> getPopup;
    const std::unique_ptr<juce::Drawable> popupArrow { style == Style::menuStripDropdown ? juce::Drawable::createFromSVG(*juce::XmlDocument::parse(BinaryData::arrow_down_white_svg)) : nullptr };

    void updatePopupArrowAlpha();
};
