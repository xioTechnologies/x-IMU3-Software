#pragma once

#include "../ApplicationSettings.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/IconButton.h"
#include "Widgets/TerminalFeed.h"
#include "Window.h"

class SerialAccessoryTerminalWindow : public Window
{
public:
    SerialAccessoryTerminalWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_);

    ~SerialAccessoryTerminalWindow() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    static constexpr int widgetMargin = 2;

    TerminalFeed terminalFeed { juce::Colours::black };
    CustomComboBox sendValue;
    IconButton sendButton { IconButton::Style::normal, BinaryData::send_svg, 0.8f, "Send" };

    juce::ValueTree recentSends;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Recent Sends.xml");

    std::function<void(ximu3::XIMU3_SerialMessage)> callback;
    uint64_t callbackID;

    void loadRecentSends();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SerialAccessoryTerminalWindow)
};
