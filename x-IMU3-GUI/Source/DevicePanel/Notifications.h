#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"
#include "Widgets/TerminalFeed.h"

class Notifications : public juce::Component,
                      private juce::Timer,
                      private juce::ValueTree::Listener
{
public:
    Notifications();

    ~Notifications() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

    void show(const bool showOrHide);

    TerminalFeed& getFeed();

private:
    TerminalFeed feed;

    IconButton showNotificationsButton { IconButton::Style::normal,
                                         BinaryData::notification_disabled_svg, 1.0f, "Enable Pop-Up",
                                         nullptr,
                                         BinaryData::notification_enabled_svg, 1.0f, "Disable Pop-Up" };
    IconButton closeButton { IconButton::Style::normal, BinaryData::close_svg, 0.8f, "Close" };
    SimpleLabel title { "Notification And Error Messages", UIFonts::smallFont };

    static constexpr int showDuration = 5000;
    static constexpr int fadeDuration = 250;

    void timerCallback() override;

    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Notifications)
};
