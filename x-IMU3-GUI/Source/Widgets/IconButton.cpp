#include "../CustomLookAndFeel.h"
#include "IconButton.h"

IconButton::IconButton(const Style style_,
                       const juce::String& icon, const float scale_, const juce::String& tooltip_,
                       std::function<juce::PopupMenu()> getPopup_,
                       const juce::String& iconOn, const float scaleOn_, const juce::String& tooltipOn_)
        : juce::DrawableButton("", ImageOnButtonBackground),
          style(style_),
          scale(scale_), scaleOn(scaleOn_),
          tooltip(tooltip_), tooltipOn(tooltipOn_.isNotEmpty() ? tooltipOn_ : tooltip_),
          getPopup(std::move(getPopup_))
{
    switch (style)
    {
        case Style::normal:
            setColour(juce::TextButton::buttonColourId, {});
            setColour(juce::TextButton::buttonOnColourId, {});
            break;
        case Style::menuStrip:
        case Style::menuStripDropdown:
            setColour(juce::TextButton::buttonColourId, UIColours::menuStripButton);
            setColour(juce::TextButton::buttonOnColourId, UIColours::menuStripButton);
            break;
    }

    setIcon(icon, iconOn);
    setTooltip(tooltip);
    if (popupArrow != nullptr)
    {
        addAndMakeVisible(*popupArrow);
    }
    setTriggeredOnMouseDown(true);
}

juce::Rectangle<float> IconButton::getImageBounds() const
{
    return getLocalBounds().toFloat().withSizeKeepingCentre((getWidth() - 6) * (getToggleState() ? scaleOn : scale),
                                                            (getHeight() - 6) * (getToggleState() ? scaleOn : scale))
                           .translated(popupArrow != nullptr ? -7.5f : 0.0f, 0.0f);
}

void IconButton::buttonStateChanged()
{
    juce::DrawableButton::buttonStateChanged();
    updatePopupArrowAlpha();
}

void IconButton::resized()
{
    juce::DrawableButton::resized();
    if (popupArrow)
    {
        popupArrow->setTransformToFit({ getWidth() - 15.0f, 6.5, 9, 11 }, juce::RectanglePlacement::xRight);
    }
}

void IconButton::enablementChanged()
{
    juce::DrawableButton::enablementChanged();
    updatePopupArrowAlpha();
}

void IconButton::clicked()
{
    if (getPopup != nullptr)
    {
        getPopup().showAt(getScreenBounds());
    }
    setTooltip(getToggleState() ? tooltipOn : tooltip);
}

void IconButton::setIcon(const juce::String& icon, const juce::String& iconOn)
{
    const auto normal = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(icon));
    const auto normalOn = iconOn.isNotEmpty() ? juce::Drawable::createFromSVG(*juce::XmlDocument::parse(iconOn)) : normal->createCopy();
    setImages(normal.get(), nullptr, nullptr, nullptr, normalOn.get());
}

void IconButton::updatePopupArrowAlpha()
{
    if (popupArrow != nullptr)
    {
        popupArrow->setAlpha((isEnabled() && getState() != buttonDown) ? 1.0f : 0.5f);
    }
}
