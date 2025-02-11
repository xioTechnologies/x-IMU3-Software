#pragma once

#include "ApplicationSettings.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/IconButton.h"
#include "Widgets/SerialAccessoryTerminal.h"
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
    SerialAccessoryTerminal serialAccessoryTerminal;
    CustomComboBox sendValue;
    IconButton sendButton { BinaryData::send_svg, "Send" };

    juce::ValueTree recentSerialAccessoryData;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Recent Serial Accessory Data.xml");

    std::function<void(ximu3::XIMU3_SerialAccessoryMessage)> callback;
    uint64_t callbackID;

    static std::string escapedToBytes(const std::string& escaped);

    static std::string bytesToEscaped(const std::string& bytes);

    static std::string jsonToBytes(std::string json);

    static std::string bytesToJson(const std::string& bytes);

    void loadRecents();

    juce::PopupMenu getMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SerialAccessoryTerminalWindow)
};
