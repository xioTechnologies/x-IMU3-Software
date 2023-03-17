#include "../CustomLookAndFeel.h"
#include "IconButton.h"

IconButton::IconButton(const juce::String& icon, const juce::String& tooltip_,
                       std::function<juce::PopupMenu()> getPopup_,
                       const bool showPopupArrow,
                       const juce::String& iconOn, const juce::String& tooltipOn_)
        : juce::DrawableButton("", ImageOnButtonBackground),
          tooltip(tooltip_), tooltipOn(tooltipOn_.isNotEmpty() ? tooltipOn_ : tooltip_),
          getPopup(std::move(getPopup_)),
          popupArrow((getPopup != nullptr && showPopupArrow) ? juce::Drawable::createFromSVG(*juce::XmlDocument::parse(BinaryData::arrow_down_white_svg)) : nullptr)
{
    setColour(juce::TextButton::buttonColourId, {});
    setColour(juce::TextButton::buttonOnColourId, {});

    setIcon(icon, iconOn);
    setTriggeredOnMouseDown(true);
}

void IconButton::paint(juce::Graphics& g)
{
    juce::DrawableButton::paint(g);

    if (popupArrow != nullptr)
    {
        popupArrow->drawWithin(g, popupArrowBounds, juce::RectanglePlacement::centred, isEnabled() ? 1.0f : 0.4f);
    }
}

juce::Rectangle<float> IconButton::getImageBounds() const
{
    return imageBounds;
}

void IconButton::resized()
{
    static constexpr auto margin = 3.0f;

    auto bounds = getLocalBounds().toFloat();
    bounds = bounds.reduced(margin);

    const auto iconSize = bounds.getHeight();

    if (popupArrow == nullptr)
    {
        imageBounds = bounds;
    }
    else
    {
        const auto arrowSize = 8.0f;
        const auto arrowMargin = margin;
        const auto iconAndArrowWidth = iconSize + arrowMargin + arrowSize;
        bounds = bounds.withSizeKeepingCentre(iconAndArrowWidth, bounds.getHeight());

        imageBounds = bounds.removeFromLeft(bounds.getHeight());
        bounds.removeFromLeft(arrowMargin);

        popupArrowBounds = bounds.withSizeKeepingCentre(arrowSize, arrowSize);
    }

    juce::DrawableButton::resized();
}

void IconButton::clicked()
{
    if (getPopup != nullptr)
    {
        getPopup().showAt(getScreenBounds());
    }
}

juce::String IconButton::getTooltip()
{
    return getToggleState() ? tooltipOn : tooltip;
}

void IconButton::setIcon(const juce::String& icon, const juce::String& iconOn)
{
    const auto normal = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(icon));
    const auto normalOn = iconOn.isNotEmpty() ? juce::Drawable::createFromSVG(*juce::XmlDocument::parse(iconOn)) : normal->createCopy();
    setImages(normal.get(), nullptr, nullptr, nullptr, normalOn.get());
}
