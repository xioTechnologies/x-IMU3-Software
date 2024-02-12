#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace UIColours
{
    static const juce::Colour backgroundDarkest { 0xFF1E1E1E };
    static const juce::Colour backgroundDark { 0xFF252526 };
    static const juce::Colour backgroundLight { 0xFF2D2D30 };
    static const juce::Colour backgroundLightest { 0xFF3E3E42 };
    static const juce::Colour foreground { 0xFFE4E4E4 };
    static const juce::Colour highlight { 0xFF3D8A9A };
    static const juce::Colour warning { 0xFFF7B500 };
    static const juce::Colour error { 0xFFEF5350 };
    static const juce::Colour update { 0xFF32C5FF };

    static const juce::Colour graphX { 0xFFE02020 };
    static const juce::Colour graphY { 0xFF6DD400 };
    static const juce::Colour graphZ { 0xFF32C5FF };
    static const juce::Colour graphChannel1 { 0xFFFFFF00 };
    static const juce::Colour graphChannel2 { 0xFFFE6ABC };
    static const juce::Colour graphChannel3 { 0xFFD7FFD9 };
    static const juce::Colour graphChannel4 { 0xFF8481FF };
    static const juce::Colour graphChannel5 { 0xFF02FFFF };
    static const juce::Colour graphChannel6 { 0xFF02C100 };
    static const juce::Colour graphChannel7 { 0xFFFE0000 };
    static const juce::Colour graphChannel8 { 0xFFFF8000 };

    static const std::vector<juce::Colour> tags =
            {
                    juce::Colour(0xFF000000),
                    juce::Colour(0xFF964B00),
                    juce::Colour(0xFFFF0000),
                    juce::Colour(0xFFFFA500),
                    juce::Colour(0xFFFFFF00),
                    juce::Colour(0xFF9ACD32),
                    juce::Colour(0xFF6495ED),
                    juce::Colour(0xFF9400D3),
                    juce::Colour(0xFFA0A0A0),
                    juce::Colour(0xFFFFFFFF),
                    juce::Colour(0xFF0C2A51),
                    juce::Colour(0xFF23869A),
                    juce::Colour(0xFF5AFBF1),
                    juce::Colour(0xFF41993C),
                    juce::Colour(0xFFFFE057),
                    juce::Colour(0xFFEE672B),
                    juce::Colour(0xFFD1366C),
                    juce::Colour(0xFFFF9CAA),
                    juce::Colour(0xFFFFF0C4),
                    juce::Colour(0xFFD19B1D),
            };
}

namespace UILayout
{
    static constexpr int panelMargin = 3;
    static constexpr int textComponentHeight = 26;
    static constexpr int tagWidth = 4;
}

namespace UIFonts
{
    namespace Typefaces
    {
        juce::Typeface::Ptr getMontserratMedium();

        juce::Typeface::Ptr getRobotoMonoRegular();
    }

#if JUCE_WINDOWS
    static constexpr auto montserratAdjustment = 2.0f;
#else
    static constexpr auto montserratAdjustment = 0.0f;
#endif

    juce::Font getDefaultFont();

    juce::Font getSmallFont();

    juce::Font getTerminalFeedFont();
}

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();

    void drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y, int width, int height,
                       bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool, bool) override;

    int getDefaultScrollbarWidth() override;

    void drawButtonBackground(juce::Graphics& g,
                              juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    juce::Font getTextButtonFont(juce::TextButton&, int) override;

    void fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& textEditor) override;

    void drawTreeviewPlusMinusBox(juce::Graphics& g, const juce::Rectangle<float>& area, juce::Colour, bool isOpen, bool isMouseOver) override;

    bool areLinesDrawnForTreeView(juce::TreeView&) override;

    int getTreeViewIndentSize(juce::TreeView&) override;

    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           const bool isSeparator, const bool isActive,
                           const bool isHighlighted, const bool isTicked,
                           const bool hasSubMenu, const juce::String& text,
                           const juce::String& shortcutKeyText,
                           const juce::Drawable* icon, const juce::Colour* const textColourToUse) override;

    void drawPopupMenuSectionHeader(juce::Graphics& g, const juce::Rectangle<int>& area, const juce::String& sectionName) override;

    juce::Font getPopupMenuFont() override;

    void getIdealPopupMenuItemSize(const juce::String& text, const bool isSeparator, int standardMenuItemHeight, int& idealWidth, int& idealHeight) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box) override;

    juce::Font getComboBoxFont(juce::ComboBox&) override;

    void drawDocumentWindowTitleBar(juce::DocumentWindow& window, juce::Graphics& g,
                                    int w, int h, int titleSpaceX, int titleSpaceW,
                                    const juce::Image* icon, bool drawTitleTextOnLeft) override;

    juce::Rectangle<int> getTooltipBounds(const juce::String& tipText, juce::Point<int> screenPos, juce::Rectangle<int> parentArea) override;

    void drawTooltip(juce::Graphics& g, const juce::String& text, int width, int height) override;

    void drawGroupComponentOutline(juce::Graphics& g, int width, int height, const juce::String& text, const juce::Justification& position, juce::GroupComponent& group) override;

    juce::Path getTickShape(float height) override;

private:
    static juce::TextLayout layoutTooltipText(const juce::String& text, const juce::Colour& colour);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};
