#include "ConnectionPanel/ConnectionPanel.h"
#include "Dialogs/SendingCommandDialog.h"
#include "SerialAccessoryTerminalWindow.h"

SerialAccessoryTerminalWindow::SerialAccessoryTerminalWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_)
        : Window(windowLayout_, type_, connectionPanel_, "Serial Accessory Terminal Menu")
{
    addAndMakeVisible(serialAccessoryTerminal);
    serialAccessoryTerminal.addMouseListener(this, true);

    addAndMakeVisible(sendValue);
    sendValue.setEditableText(true);

    loadRecents();

    addAndMakeVisible(sendButton);
    sendButton.onClick = [this]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(CommandMessage("accessory", removeEscapeCharacters(sendValue.getText())), std::vector<ConnectionPanel*> { &connectionPanel }));

        serialAccessoryTerminal.add(uint64_t(-1), removeEscapeCharacters(sendValue.getText()));

        for (const auto data : recentSerialAccessoryData)
        {
            if (data["data"] == sendValue.getText())
            {
                recentSerialAccessoryData.removeChild(data, nullptr);
                break;
            }
        }

        while (recentSerialAccessoryData.getNumChildren() >= 12)
        {
            recentSerialAccessoryData.removeChild(recentSerialAccessoryData.getChild(recentSerialAccessoryData.getNumChildren() - 1), nullptr);
        }

        recentSerialAccessoryData.addChild({ "Data", {{ "data", sendValue.getText() }}}, 0, nullptr);
        file.replaceWithText(recentSerialAccessoryData.toXmlString());

        loadRecents();
    };

    callbackID = connectionPanel.getConnection()->addSerialAccessoryCallback(callback = [&, self = SafePointer<juce::Component>(this)](auto message)
    {
        juce::MessageManager::callAsync([&, self, message]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            serialAccessoryTerminal.add(message.timestamp, juce::String::createStringFromData(message.char_array, (int) message.number_of_bytes));
                                        });
    });
}

SerialAccessoryTerminalWindow::~SerialAccessoryTerminalWindow()
{
    connectionPanel.getConnection()->removeCallback(callbackID);
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

void SerialAccessoryTerminalWindow::mouseDown(const juce::MouseEvent& mouseEvent)
{
    if (mouseEvent.mods.isPopupMenu())
    {
        getMenu().show();
    }
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

void SerialAccessoryTerminalWindow::loadRecents()
{
    recentSerialAccessoryData = juce::ValueTree::fromXml(file.loadFileAsString());
    if (recentSerialAccessoryData.isValid() == false)
    {
        recentSerialAccessoryData = juce::ValueTree("RecentSerialAccessoryData");
    }

    sendValue.clear(juce::dontSendNotification);
    for (const auto data : recentSerialAccessoryData)
    {
        sendValue.addItem(data["data"], sendValue.getNumItems() + 1);
    }

    sendValue.setText(sendValue.getNumItems() > 0 ? sendValue.getItemText(0) : "Hello World!", juce::dontSendNotification);
}

juce::PopupMenu SerialAccessoryTerminalWindow::getMenu()
{
    juce::PopupMenu menu = Window::getMenu();
    menu.addItem("Copy to Clipboard", [&]
    {
        serialAccessoryTerminal.copyToClipboard();
    });
    menu.addItem("Clear", [&]
    {
        serialAccessoryTerminal.clearAll();
    });
    return menu;
}
