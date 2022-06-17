#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
    setDefaultSansSerifTypeface(UIFonts::Typefaces::montserratMedium); // progress bar only

    setColour(juce::CaretComponent::caretColourId, UIColours::background);
    setColour(juce::ComboBox::backgroundColourId, UIColours::textEditor);
    setColour(juce::ComboBox::textColourId, UIColours::background);
    setColour(juce::ComboBox::arrowColourId, juce::Colours::grey);
    setColour(juce::GroupComponent::outlineColourId, juce::Colours::grey);
    setColour(juce::GroupComponent::textColourId, juce::Colours::white);
    setColour(juce::Label::textColourId, juce::Colours::white);
    setColour(juce::Label::textWhenEditingColourId, UIColours::menuStrip);
    setColour(juce::Label::backgroundWhenEditingColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
    setColour(juce::ListBox::backgroundColourId, {});
    setColour(juce::PopupMenu::highlightedBackgroundColourId, UIColours::highlight);
    setColour(juce::PopupMenu::backgroundColourId, UIColours::background);
    setColour(juce::PopupMenu::headerTextColourId, juce::Colours::grey);
    setColour(juce::PopupMenu::textColourId, juce::Colours::white);
    setColour(juce::ResizableWindow::backgroundColourId, UIColours::menuStrip);
    setColour(juce::ScrollBar::thumbColourId, UIColours::menuStrip);
    setColour(juce::ScrollBar::backgroundColourId, UIColours::footer);
    setColour(juce::TextButton::buttonOnColourId, UIColours::textEditor);
    setColour(juce::TextButton::textColourOnId, UIColours::background);
    setColour(juce::TextButton::buttonColourId, UIColours::menuStripButton);
    setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    setColour(juce::TextEditor::backgroundColourId, UIColours::textEditor);
    setColour(juce::TextEditor::textColourId, UIColours::background);
    setColour(juce::TextEditor::highlightedTextColourId, UIColours::textEditor);
    setColour(juce::TextEditor::highlightColourId, UIColours::highlight);
    setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::white);
    setColour(juce::ToggleButton::tickColourId, UIColours::menuStrip);
    setColour(juce::TooltipWindow::textColourId, juce::Colours::white);
    setColour(juce::TooltipWindow::outlineColourId, juce::Colours::grey);
    setColour(juce::TooltipWindow::backgroundColourId, UIColours::background);
}

void CustomLookAndFeel::drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y, int width, int height,
                                      bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool, bool)
{
    juce::Rectangle<int> thumbBounds;

    if (isScrollbarVertical)
    {
        thumbBounds = { x, thumbStartPosition, width, thumbSize };
    }
    else
    {
        thumbBounds = { thumbStartPosition, y, thumbSize, height };
    }

    g.setColour(scrollbar.findColour(juce::ScrollBar::ColourIds::backgroundColourId));
    g.fillRoundedRectangle(juce::Rectangle<int>(x, y, width, height).toFloat(), 2.0f);
    g.setColour(scrollbar.findColour(juce::ScrollBar::ColourIds::thumbColourId));
    g.fillRoundedRectangle(thumbBounds.reduced(2).toFloat(), 1.0f);
}

int CustomLookAndFeel::getMinimumScrollbarThumbSize(juce::ScrollBar& scrollbar)
{
    return juce::jmin(scrollbar.getWidth(), scrollbar.getHeight()) * 4;
}

int CustomLookAndFeel::getDefaultScrollbarWidth()
{
    return 10;
}

void CustomLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                             juce::Button& button,
                                             const juce::Colour& backgroundColour,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown)
{
    auto cornerSize = 3.0f;
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

    auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                                      .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.1f);

    if (shouldDrawButtonAsDown && button.isEnabled())
    {
        baseColour = baseColour.contrasting(0.1f);
    }

    if (shouldDrawButtonAsHighlighted)
    {
        baseColour = baseColour.contrasting(0.1f);
    }

    g.setColour(baseColour);

    if (button.isConnectedOnLeft() || button.isConnectedOnRight())
    {
        juce::Path path;
        path.addRoundedRectangle(bounds.getX(),
                                 bounds.getY(),
                                 bounds.getWidth(),
                                 bounds.getHeight(),
                                 cornerSize,
                                 cornerSize,
                                 !button.isConnectedOnLeft(),
                                 !button.isConnectedOnRight(),
                                 !button.isConnectedOnLeft(),
                                 !button.isConnectedOnRight());

        g.fillPath(path);
    }
    else
    {
        g.fillRoundedRectangle(bounds, cornerSize);
    }
}

juce::Font CustomLookAndFeel::getTextButtonFont(juce::TextButton&, int)
{
    return UIFonts::defaultFont;
}

void CustomLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button, bool, bool)
{
    juce::Font font(getTextButtonFont(button, button.getHeight()));
    g.setFont(font);
    g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                          : juce::TextButton::textColourOffId)
                      .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

    const int yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
    const int cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;

    const int fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
    const int leftIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    const int rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    const int textWidth = button.getWidth() - leftIndent - rightIndent;

    if (textWidth > 0)
    {
        // Adjustment: fix stretching glitch
        g.drawText(button.getButtonText(),
                   leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                   juce::Justification::centred, true);
    }
}

void CustomLookAndFeel::fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor)
{
    auto backgroundColour = textEditor.findColour(juce::TextEditor::backgroundColourId);
    if (!textEditor.isEnabled() || textEditor.isReadOnly())
    {
        backgroundColour = backgroundColour.withAlpha(0.5f);
    }

    if (dynamic_cast<juce::AlertWindow*> (textEditor.getParentComponent()) != nullptr)
    {
        g.setColour(backgroundColour);
        g.fillRect(0, 0, width, height);

        g.setColour(textEditor.findColour(juce::TextEditor::outlineColourId));
        g.drawHorizontalLine(height - 1, 0.0f, (float) width);
    }
    else
    {
        const auto cornerSize = 3.0f;
        g.setColour(backgroundColour);
        g.fillRoundedRectangle(0, 0, (float) width, (float) height, cornerSize);
    }
}

void CustomLookAndFeel::drawTreeviewPlusMinusBox(juce::Graphics& g, const juce::Rectangle<float>& area, juce::Colour, bool isOpen, bool isMouseOver)
{
    static const std::unique_ptr<juce::Drawable> arrowDownWhite { juce::Drawable::createFromSVG(*juce::XmlDocument::parse(BinaryData::arrow_down_white_svg)) };
    static const std::unique_ptr<juce::Drawable> arrowDownGrey { juce::Drawable::createFromSVG(*juce::XmlDocument::parse(BinaryData::arrow_down_grey_svg)) };
    static const std::unique_ptr<juce::Drawable> arrowRightWhite { juce::Drawable::createFromSVG(*juce::XmlDocument::parse(BinaryData::arrow_right_white_svg)) };
    static const std::unique_ptr<juce::Drawable> arrowRightGrey { juce::Drawable::createFromSVG(*juce::XmlDocument::parse(BinaryData::arrow_right_grey_svg)) };

    const auto* const icon = (isOpen ? (isMouseOver ? arrowDownWhite : arrowDownGrey) : (isMouseOver ? arrowRightWhite : arrowRightGrey)).get();
    icon->drawWithin(g, area.withSizeKeepingCentre(5.0f, 5.0f), juce::RectanglePlacement::centred | juce::RectanglePlacement::fillDestination, 1.0f);
}

bool CustomLookAndFeel::areLinesDrawnForTreeView(juce::TreeView&)
{
    return true;
}

int CustomLookAndFeel::getTreeViewIndentSize(juce::TreeView&)
{
    return 15;
}

void CustomLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
{
    g.fillAll(findColour(juce::PopupMenu::backgroundColourId));
    g.setColour(UIColours::menuStrip);
    g.drawRect(0, 0, width, height);
}

// This file is copied from juce_juce::LookAndFeel_V4.cpp with minor modifications
void CustomLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                          const bool isSeparator, const bool isActive,
                                          const bool isHighlighted, const bool isTicked,
                                          const bool hasSubMenu, const juce::String& text,
                                          const juce::String& shortcutKeyText,
                                          const juce::Drawable* icon, const juce::Colour* const textColourToUse)
{
    if (isSeparator)
    {
        // Adjustment: Remove reduction
        auto r = area;

        // Adjustment: Decrease amount that is stripped of
        r.removeFromTop(juce::roundToInt((r.getHeight() * 0.4f) - 0.5f));

        // Adjustment: Change colour
        g.setColour(UIColours::menuStrip);
        g.fillRect(r.removeFromTop(1));
    }
    else
    {
        auto textColour = (textColourToUse == nullptr ? findColour(juce::PopupMenu::textColourId)
                                                      : *textColourToUse);

        auto r = area.reduced(1);

        if (isHighlighted && isActive)
        {
            g.setColour(findColour(juce::PopupMenu::highlightedBackgroundColourId));
            g.fillRect(r);

            g.setColour(findColour(juce::PopupMenu::highlightedTextColourId));
        }
        else
        {
            g.setColour(textColour.withMultipliedAlpha(isActive ? 1.0f : 0.5f));
        }

        r.reduce(juce::jmin(5, area.getWidth() / 20), 0);

        auto font = getPopupMenuFont();

        auto maxFontHeight = r.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight)
        {
            font.setHeight(maxFontHeight);
        }

        g.setFont(font);

        auto iconArea = r.removeFromLeft(juce::roundToInt(maxFontHeight)).toFloat();

        // Adjustment: Move icon to left
        iconArea.translate(-(maxFontHeight / 5), 0);

        if (icon != nullptr)
        {
            // Adjustment: Change icon position to position the colour tags in disconnect menu correctly
            iconArea = icon->getDrawableBounds().withPosition(0, 0);
            icon->drawWithin(g, iconArea, juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize, 1.0f);
            r.removeFromLeft(juce::roundToInt(maxFontHeight * 0.5f));
        }
        else if (isTicked)
        {
            auto tick = getTickShape(1.0f);

            // Adjustment: Change tick target bounds
            g.fillPath(tick, tick.getTransformToScaleToFit(iconArea.reduced(
                    std::min(getPopupMenuFont().getHeight(), maxFontHeight) / 10).toFloat(), true));
        }

        if (hasSubMenu)
        {
            auto arrowH = 0.6f * getPopupMenuFont().getAscent();

            auto x = (float) r.removeFromRight((int) arrowH).getX();
            auto halfH = (float) r.getCentreY();

            juce::Path path;
            path.startNewSubPath(x, halfH - arrowH * 0.5f);
            path.lineTo(x + arrowH * 0.6f, halfH);
            path.lineTo(x, halfH + arrowH * 0.5f);

            g.strokePath(path, juce::PathStrokeType(2.0f));
        }

        r.removeFromRight(3);
        g.drawFittedText(text, r, juce::Justification::centredLeft, 1);

        if (shortcutKeyText.isNotEmpty())
        {
            auto f2 = font;
            f2.setHeight(f2.getHeight() * 0.75f);
            f2.setHorizontalScale(0.95f);
            g.setFont(f2);

            g.drawText(shortcutKeyText, r, juce::Justification::centredRight, true);
        }
    }
}

void CustomLookAndFeel::drawPopupMenuSectionHeader(juce::Graphics& g, const juce::Rectangle<int>& area, const juce::String& sectionName)
{
    g.setFont(getPopupMenuFont());
    g.setColour(findColour(juce::PopupMenu::headerTextColourId));
    g.drawFittedText(sectionName,
                     area.getX() + 19, area.getY(), area.getWidth() - 16, juce::roundToInt((area.getHeight() * 0.8f)),
                     juce::Justification::centredLeft, 1);
}

juce::Font CustomLookAndFeel::getPopupMenuFont()
{
    return UIFonts::defaultFont;
}

void CustomLookAndFeel::getIdealPopupMenuItemSize(const juce::String& text, const bool isSeparator, int standardMenuItemHeight, int& idealWidth, int& idealHeight)
{
    if (isSeparator)
    {
        idealWidth = 50;
        idealHeight = standardMenuItemHeight > 0 ? standardMenuItemHeight / 10 : 10;
    }
    else
    {
        auto font = getPopupMenuFont();

        if (standardMenuItemHeight > 0 && font.getHeight() > (float) standardMenuItemHeight / 1.3f)
        {
            font.setHeight((float) standardMenuItemHeight / 1.3f);
        }

        idealHeight = standardMenuItemHeight > 0 ? standardMenuItemHeight : (juce::roundToInt(font.getHeight() * 1.3f) + 2);
        idealWidth = font.getStringWidth(text) + idealHeight * 2;
    }
}

void CustomLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box)
{
    auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    juce::Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

    g.setColour(box.findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

    g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
    const auto arrowZone = boxBounds.removeFromRight(boxBounds.getHeight()).reduced(boxBounds.getHeight() / 3);

    static const std::unique_ptr<juce::Drawable> arrow { juce::Drawable::createFromSVG(*juce::XmlDocument::parse(BinaryData::arrow_down_grey_svg)) };
    arrow->drawWithin(g, arrowZone.toFloat(), juce::RectanglePlacement::centred, 1.0f);
}

juce::Font CustomLookAndFeel::getComboBoxFont(juce::ComboBox&)
{
    return UIFonts::defaultFont;
}

void CustomLookAndFeel::drawDocumentWindowTitleBar(juce::DocumentWindow& window, juce::Graphics& g,
                                                   int w, int h, int, int,
                                                   const juce::Image* icon, bool)
{
    g.fillAll(UIColours::menuStripButton);

    juce::Rectangle<int> bounds(w, h);

    if (icon != nullptr)
    {
        g.drawImage(*icon, bounds.removeFromLeft(h).withSizeKeepingCentre(22, 22).toFloat(), juce::RectanglePlacement::centred);
    }

    const auto font = UIFonts::defaultFont;
    g.setFont(font);
    g.setColour(juce::Colours::white);
    g.drawText(window.getName(), bounds, juce::Justification::left, true);
}

juce::Rectangle<int> CustomLookAndFeel::getTooltipBounds(const juce::String& tipText, juce::Point<int> screenPos, juce::Rectangle<int> parentArea)
{
    const juce::TextLayout tl(layoutTooltipText(tipText, juce::Colours::black));

    auto w = juce::roundToInt(tl.getWidth() + 24.0f);
    auto h = juce::roundToInt(tl.getHeight() + 12.0f);

    return juce::Rectangle<int>(screenPos.x > parentArea.getCentreX() ? screenPos.x - (w + 6) : screenPos.x + 12,
                                screenPos.y > parentArea.getCentreY() ? screenPos.y - (h + 12) : screenPos.y + 12,
                                w, h)
            .constrainedWithin(parentArea);
}

void CustomLookAndFeel::drawTooltip(juce::Graphics& g, const juce::String& text, int width, int height)
{
    juce::Rectangle<int> bounds(width, height);
    auto cornerSize = 5.0f;

    g.setColour(findColour(juce::TooltipWindow::backgroundColourId));
    g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

    g.setColour(findColour(juce::TooltipWindow::outlineColourId));
    g.drawRoundedRectangle(bounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

    layoutTooltipText(text, findColour(juce::TooltipWindow::textColourId))
            .draw(g, { (float) width, (float) height });
}

void CustomLookAndFeel::drawGroupComponentOutline(juce::Graphics& g, int width, int height, const juce::String& text, const juce::Justification& position, juce::GroupComponent& group)
{
    const float indent = 3.0f;
    const float textEdgeGap = 4.0f;
    auto cs = 5.0f;

    // Adjustment
    const auto font = UIFonts::defaultFont;

    juce::Path p;
    auto x = indent;
    auto y = font.getAscent() - 3.0f;
    auto w = juce::jmax(0.0f, (float) width - x * 2.0f);
    auto h = juce::jmax(0.0f, (float) height - y - indent);
    cs = juce::jmin(cs, w * 0.5f, h * 0.5f);
    auto cs2 = 2.0f * cs;

    auto textW = text.isEmpty() ? 0
                                : juce::jlimit(0.0f,
                                               juce::jmax(0.0f, w - cs2 - textEdgeGap * 2),
                                               (float) font.getStringWidth(text) + textEdgeGap * 2.0f);
    auto textX = cs + textEdgeGap;

    if (position.testFlags(juce::Justification::horizontallyCentred))
    {
        textX = cs + (w - cs2 - textW) * 0.5f;
    }
    else if (position.testFlags(juce::Justification::right))
    {
        textX = w - cs - textW - textEdgeGap;
    }

    p.startNewSubPath(x + textX + textW, y);
    p.lineTo(x + w - cs, y);

    p.addArc(x + w - cs2, y, cs2, cs2, 0, juce::MathConstants<float>::halfPi);
    p.lineTo(x + w, y + h - cs);

    p.addArc(x + w - cs2, y + h - cs2, cs2, cs2, juce::MathConstants<float>::halfPi, juce::MathConstants<float>::pi);
    p.lineTo(x + cs, y + h);

    p.addArc(x, y + h - cs2, cs2, cs2, juce::MathConstants<float>::pi, juce::MathConstants<float>::pi * 1.5f);
    p.lineTo(x, y + cs);

    p.addArc(x, y, cs2, cs2, juce::MathConstants<float>::pi * 1.5f, juce::MathConstants<float>::twoPi);
    p.lineTo(x + textX, y);

    auto alpha = group.isEnabled() ? 1.0f : 0.5f;

    g.setColour(group.findColour(juce::GroupComponent::outlineColourId)
                     .withMultipliedAlpha(alpha));

    g.strokePath(p, juce::PathStrokeType(2.0f));

    g.setColour(group.findColour(juce::GroupComponent::textColourId)
                     .withMultipliedAlpha(alpha));
    g.setFont(font);
    g.drawText(text,
               juce::roundToInt(x + textX), 0,
               juce::roundToInt(textW),
            // Adjustment
               juce::roundToInt(font.getHeight()),
               juce::Justification::centred, true);
}

juce::Path CustomLookAndFeel::getTickShape(float height)
{
    static const std::unique_ptr<juce::Drawable> icon { juce::Drawable::createFromSVG(*juce::XmlDocument::parse(BinaryData::tick_white_svg)) };

    juce::Path path(icon->getOutlineAsPath());
    path.scaleToFit(0, 0, height * 2.0f, height, true);
    return path;
}

juce::TextLayout CustomLookAndFeel::layoutTooltipText(const juce::String& text, const juce::Colour& colour)
{
    const int maxToolTipWidth = 400;

    juce::AttributedString s;
    s.setJustification(juce::Justification::centred);
    s.append(text, UIFonts::defaultFont, colour);

    juce::TextLayout tl;
    tl.createLayoutWithBalancedLineLengths(s, (float) maxToolTipWidth);
    return tl;
}
