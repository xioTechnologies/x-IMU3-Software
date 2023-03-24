#pragma once

#include "../ApplicationSettings.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/IconButton.h"
#include "Widgets/SerialAccessoryTerminal.h"
#include "Window.h"

class SerialAccessoryTerminalWindow : public Window
{
public:
    SerialAccessoryTerminalWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_);

    ~SerialAccessoryTerminalWindow() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    SerialAccessoryTerminal serialAccessoryTerminal;
    CustomComboBox sendValue;
    IconButton sendButton { BinaryData::send_svg, "Send", nullptr, false, BinaryData::send_warning_svg, "Send (Failed)" };

    juce::ValueTree sendHistory;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Send History.xml");

    std::function<void(ximu3::XIMU3_SerialAccessoryMessage)> callback;
    uint64_t callbackID;

    void loadSendHistory();

    juce::PopupMenu getMenu();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SerialAccessoryTerminalWindow)
};
