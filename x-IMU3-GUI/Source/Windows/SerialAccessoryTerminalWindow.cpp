#include "../Convert.h"
#include "DevicePanel/DevicePanel.h"
#include "SerialAccessoryTerminalWindow.h"

SerialAccessoryTerminalWindow::SerialAccessoryTerminalWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, DevicePanel& devicePanel_)
        : Window(windowLayout_, type_, devicePanel_, "Serial Accessory Terminal Menu", std::bind(&SerialAccessoryTerminalWindow::getMenu, this))
{
    addAndMakeVisible(serialAccessoryTerminal);

    addAndMakeVisible(sendValue);
    sendValue.setEditableText(true);

    loadSendHistory();

    addAndMakeVisible(sendButton);
    sendButton.onClick = [this]
    {
        sendValue.setEnabled(false);
        sendButton.setEnabled(false);
        sendButton.setToggleState(false, juce::dontSendNotification);

        serialAccessoryTerminal.add(uint64_t(-1), removeEscapeCharacters(sendValue.getText()));

        devicePanel.sendCommands({ CommandMessage("accessory", removeEscapeCharacters(sendValue.getText())) }, this, [&](const auto& responses, const auto&)
        {
            sendValue.setEnabled(true);
            sendButton.setEnabled(true);
            sendButton.setToggleState(responses.empty(), juce::dontSendNotification);
        });

        for (const auto serial : serialHistory)
        {
            if (serial.getProperty("serial") == sendValue.getText())
            {
                serialHistory.removeChild(serial, nullptr);
                break;
            }
        }

        while (serialHistory.getNumChildren() >= 12)
        {
            serialHistory.removeChild(serialHistory.getChild(serialHistory.getNumChildren() - 1), nullptr);
        }

        serialHistory.addChild({ "Serial", {{ "serial", sendValue.getText() }}}, 0, nullptr);
        file.replaceWithText(serialHistory.toXmlString());

        loadSendHistory();
    };

    callbackID = devicePanel.getConnection()->addSerialAccessoryCallback(callback = [&, self = SafePointer<juce::Component>(this)](auto message)
    {
        juce::MessageManager::callAsync([&, self, message]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            serialAccessoryTerminal.add(message.timestamp, message.char_array);
                                        });
    });
}

SerialAccessoryTerminalWindow::~SerialAccessoryTerminalWindow()
{
    devicePanel.getConnection()->removeCallback(callbackID);
}

void SerialAccessoryTerminalWindow::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::backgroundDark);
    Window::paint(g);
}

void SerialAccessoryTerminalWindow::resized()
{
    Window::resized();

    auto bounds = getContentBounds();

    auto sendCommandBounds = bounds.removeFromBottom(UILayout::textComponentHeight);
    sendButton.setBounds(sendCommandBounds.removeFromRight(40));
    sendValue.setBounds(sendCommandBounds);

    serialAccessoryTerminal.setBounds(bounds);
}

juce::String SerialAccessoryTerminalWindow::removeEscapeCharacters(const juce::String& input)
{
    juce::String output;

    for (int index = 0; index < input.length(); index++)
    {
        if (input[index] != '\\')
        {
            output += input[index];
            continue;
        }

        if (++index >= input.length())
        {
            return output; // invalid escape sequence
        }

        switch (input[index])
        {
            case '\\':
                output += '\\';
                break;

            case 'n':
                output += '\n';
                break;

            case 'r':
                output += '\r';
                break;

            case 'x':
            {
                if (index >= input.length() - 2)
                {
                    return output; // invalid escape sequence
                }

                const auto upperNibble = juce::CharacterFunctions::getHexDigitValue((juce::juce_wchar) (juce::uint8) input[++index]);
                const auto lowerNibble = juce::CharacterFunctions::getHexDigitValue((juce::juce_wchar) (juce::uint8) input[++index]);

                if (upperNibble == -1 || lowerNibble == -1)
                {
                    break; // invalid escape sequence
                }

                output += (char) ((upperNibble << 4) + lowerNibble);
                break;
            }

            default:
                break; // invalid escape sequence
        }
    }

    return output;
}

void SerialAccessoryTerminalWindow::loadSendHistory()
{
    serialHistory = juce::ValueTree::fromXml(file.loadFileAsString());
    if (serialHistory.isValid() == false)
    {
        serialHistory = juce::ValueTree("SerialHistory");
    }

    sendValue.clear(juce::dontSendNotification);

    juce::StringArray items;
    for (const auto serial : serialHistory)
    {
        items.add(serial["serial"]);
    }
    sendValue.addItemList(items, 1);

    sendValue.setText(sendValue.getNumItems() > 0 ? sendValue.getItemText(0) : "Hello World!", juce::dontSendNotification);
}

juce::PopupMenu SerialAccessoryTerminalWindow::getMenu()
{
    juce::PopupMenu menu;
    menu.addItem("Copy To Clipboard", [&]
    {
        serialAccessoryTerminal.copyToClipboard();
    });
    menu.addItem("Clear All", [&]
    {
        serialAccessoryTerminal.clearAll();
    });
    return menu;
}
