#pragma once

#include "ApplicationSettings.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomComboBox.h"
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

    // Methods for converting between three string representations: bytes,
    // escaped, and JSON.
    //
    // Bytes: Each character is stored as its raw byte between 0x00 and 0xFF.
    // Some characters may not be printable.
    //
    // Escaped: Strings used by the UI, either for display or user input.
    // Printable ASCII characters 0x20 to 0x5B and 0x5D to 0x7F are stored as
    // their raw byte values. All other characters, except 0x0A, 0x0D, and
    // 0x5C, are stored as the 4-byte escape sequence "\\xHH" where HH is the
    // 2-digit hexadecimal value.  Characters 0x0A, 0x0D, and 0x5C are stored
    // as the 2-byte sequences "\\n", "\\r", "\\\\" respectively.
    //
    // JSON: Strings follow RFC 8259 with the following clarifications: UTF-8
    // characters are not supported so that all characters are limited to the
    // range 0x00 to 0xFF. The escape sequences "\b", "\f", "\n", "\r", and
    // "\t" are preferred over their 6-byte escape sequence "\\uHHHH".
    //
    // https://datatracker.ietf.org/doc/html/rfc8259

    static std::string escapedToBytes(const std::string& escaped);

    static std::string bytesToEscaped(const std::string& bytes);

    static std::string jsonToBytes(std::string json);

    static std::string bytesToJson(const std::string& bytes);

    void loadRecents();

    juce::PopupMenu getMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SerialAccessoryTerminalWindow)
};
