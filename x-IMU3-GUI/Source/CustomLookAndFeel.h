#pragma once

#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace UIColours
{
    static const juce::Colour footer = juce::Colour::fromRGB(29, 29, 29);
    static const juce::Colour background = juce::Colour::fromRGB(40, 40, 40);
    static const juce::Colour menuStrip = juce::Colour::fromRGB(60, 63, 65);
    static const juce::Colour menuStripButton = juce::Colour::fromRGB(80, 83, 84);
    static const juce::Colour textEditor = juce::Colour::fromRGB(228, 228, 228);
    static const juce::Colour highlight = juce::Colour::fromRGB(0, 145, 255);
    static const juce::Colour hyperlink = juce::Colour::fromRGB(29, 228, 230);
    static const juce::Colour warning = juce::Colour::fromRGB(247, 181, 0);
    static const juce::Colour graphRed = juce::Colour::fromRGB(224, 32, 32);
    static const juce::Colour graphBlue = juce::Colour::fromRGB(50, 197, 255);
    static const juce::Colour graphGreen = juce::Colour::fromRGB(109, 212, 0);

    static const std::vector<juce::Colour> panelTags =
            {
                    juce::Colour::fromString("FF000000"),
                    juce::Colour::fromString("FF964B00"),
                    juce::Colour::fromString("FFFF0000"),
                    juce::Colour::fromString("FFFFA500"),
                    juce::Colour::fromString("FFFFFF00"),
                    juce::Colour::fromString("FF9ACD32"),
                    juce::Colour::fromString("FF6495ED"),
                    juce::Colour::fromString("FF9400D3"),
                    juce::Colour::fromString("FFA0A0A0"),
                    juce::Colour::fromString("FFFFFFFF"),
            };
}

namespace UILayout
{
    static int constexpr panelMargin = 3;
    static int constexpr textComponentHeight = 26;
}

namespace UIFonts
{
    namespace Typefaces
    {
        static const juce::Typeface::Ptr montserratMedium = juce::Typeface::createSystemTypefaceFor(BinaryData::MontserratMedium_ttf, BinaryData::MontserratMedium_ttfSize);
        static const juce::Typeface::Ptr robotoMonoRegular = juce::Typeface::createSystemTypefaceFor(BinaryData::RobotoMonoRegular_ttf, BinaryData::RobotoMonoRegular_ttfSize);
    }

#if JUCE_WINDOWS
    static constexpr auto montserratAdjustment = 2.0f;
#else
    static constexpr auto montserratAdjustment = 0.0f;
#endif

    static const juce::Font defaultFont = juce::Font(Typefaces::montserratMedium).withHeight(15.0f + montserratAdjustment);
    static const juce::Font smallFont = juce::Font(Typefaces::montserratMedium).withHeight(13.0f + montserratAdjustment);
    static const juce::Font terminalFeedFont = juce::Font(Typefaces::robotoMonoRegular).withHeight(15.0f);
}

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();

    void drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y, int width, int height,
                       bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool, bool) override;

    int getMinimumScrollbarThumbSize(juce::ScrollBar& scrollbar) override;

    int getDefaultScrollbarWidth() override;

    void drawButtonBackground(juce::Graphics& g,
                              juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    juce::Font getTextButtonFont(juce::TextButton&, int) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool, bool) override;

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
