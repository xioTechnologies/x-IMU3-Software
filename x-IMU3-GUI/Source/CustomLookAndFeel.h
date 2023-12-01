#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace UIColours
{
    static const juce::Colour backgroundDarkest = juce::Colour::fromRGB(30, 30, 30);
    static const juce::Colour backgroundDark = juce::Colour::fromRGB(37, 37, 38);
    static const juce::Colour backgroundLight = juce::Colour::fromRGB(45, 45, 48);
    static const juce::Colour backgroundLightest = juce::Colour::fromRGB(62, 62, 66);
    static const juce::Colour foreground = juce::Colour::fromRGB(228, 228, 228);
    static const juce::Colour highlight = juce::Colour::fromRGB(61, 138, 154);
    static const juce::Colour warning = juce::Colour::fromRGB(247, 181, 0);

    static const juce::Colour graphX = juce::Colour::fromRGB(224, 32, 32);
    static const juce::Colour graphY = juce::Colour::fromRGB(109, 212, 0);
    static const juce::Colour graphZ = juce::Colour::fromRGB(50, 197, 255);
    static const juce::Colour graphChannel1 = juce::Colour::fromRGB(255, 255, 0);
    static const juce::Colour graphChannel2 = juce::Colour::fromRGB(254, 106, 188);
    static const juce::Colour graphChannel3 = juce::Colour::fromRGB(215, 255, 217);
    static const juce::Colour graphChannel4 = juce::Colour::fromRGB(132, 129, 255);
    static const juce::Colour graphChannel5 = juce::Colour::fromRGB(2, 255, 255);
    static const juce::Colour graphChannel6 = juce::Colour::fromRGB(2, 193, 0);
    static const juce::Colour graphChannel7 = juce::Colour::fromRGB(254, 0, 0);
    static const juce::Colour graphChannel8 = juce::Colour::fromRGB(255, 128, 0);

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
