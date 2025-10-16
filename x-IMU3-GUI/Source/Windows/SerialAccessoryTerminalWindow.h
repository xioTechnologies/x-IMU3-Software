#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/SerialAccessoryTerminal.h"
#include "Widgets/SerialAccessoryTextEditor.h"
#include "Window.h"

class SerialAccessoryTerminalWindow : public Window
{
public:
    SerialAccessoryTerminalWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, ConnectionPanel& connectionPanel_);

    ~SerialAccessoryTerminalWindow() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

    void mouseDown(const juce::MouseEvent& mouseEvent) override;

private:
    SerialAccessoryTerminal terminal;
    SerialAccessoryTextEditor textEditor;

    std::function<void(ximu3::XIMU3_SerialAccessoryMessage)> callback;
    uint64_t callbackId;

    juce::PopupMenu getMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SerialAccessoryTerminalWindow)
};
