#include "../CustomLookAndFeel.h"
#include "CustomComboBox.h"

class CustomComboBox::LookAndFeel : public CustomLookAndFeel
{
public:
    LookAndFeel()
    {
        setColour(juce::PopupMenu::backgroundColourId, UIColours::textEditor);
        setColour(juce::PopupMenu::textColourId, UIColours::backgroundDark);
    }

    void drawLabel(juce::Graphics& g, juce::Label& label_) override
    {
        const auto cornerSize = 3.0f;
        const auto lineThickness = 1.0f;

        g.setColour(label_.findColour(juce::Label::backgroundColourId).withMultipliedAlpha(label_.isEnabled() ? 1.0f : 0.5f));
        g.fillRoundedRectangle(label_.getLocalBounds().toFloat(), cornerSize);

        if (label_.isBeingEdited() == false)
        {
            g.setColour(label_.findColour(juce::Label::textColourId).withMultipliedAlpha(label_.isEnabled() ? 1.0f : 0.5f));
            g.setFont(getLabelFont(label_));
            const auto textArea = getLabelBorderSize(label_).subtractedFrom(label_.getLocalBounds());
            g.drawText(label_.getText(), textArea, label_.getJustificationType(), false);

            g.setColour(label_.findColour(juce::Label::outlineColourId).withMultipliedAlpha(label_.isEnabled() ? 1.0f : 0.5f));
        }
        else if (label_.isEnabled())
        {
            g.setColour(label_.findColour(juce::Label::outlineColourId).withMultipliedAlpha(label_.isEnabled() ? 1.0f : 0.5f));
        }

        g.drawRoundedRectangle(label_.getLocalBounds().toFloat(), cornerSize, lineThickness);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeel)
};

CustomComboBox::CustomComboBox()
{
    lookAndFeel = std::make_unique<LookAndFeel>();
    setLookAndFeel(lookAndFeel.get());

    setTextWhenNoChoicesAvailable("");
}

CustomComboBox::~CustomComboBox()
{
    setLookAndFeel(nullptr);
}

bool CustomComboBox::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::returnKey) // forward to parent
    {
        return false;
    }

    return juce::ComboBox::keyPressed(key);
}
