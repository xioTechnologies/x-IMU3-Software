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
        CommandMessage commandMessage("{\"accessory\":" + bytesToJson(escapedToBytes(sendValue.getText().toStdString())) + "}");
        if (sendValue.getText().isEmpty() || commandMessage.json.empty())
        {
            serialAccessoryTerminal.addError(sendValue.getText());
            return;
        }

        DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(commandMessage, std::vector<ConnectionPanel*> { &connectionPanel }));

        serialAccessoryTerminal.addTX(bytesToEscaped(jsonToBytes(commandMessage.value)));

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

        recentSerialAccessoryData.addChild({ "Data", { { "data", sendValue.getText() } } }, 0, nullptr);
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

            serialAccessoryTerminal.addRX(message.timestamp, bytesToEscaped({ message.char_array, (unsigned int) message.number_of_bytes }));
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
        getMenu().showMenuAsync({});
    }
}

std::string SerialAccessoryTerminalWindow::escapedToBytes(const std::string& escaped)
{
    std::string bytes;

    for (size_t index = 0; index < escaped.length(); index++)
    {
        if (escaped[index] != '\\')
        {
            bytes += escaped[index];
            continue;
        }

        if (++index == escaped.length())
        {
            break;
        }

        switch (escaped[index])
        {
            case '"':
                bytes += '"';
                continue;

            case '\\':
                bytes += '\\';
                continue;

            case 'b':
                bytes += '\b';
                continue;

            case 'f':
                bytes += '\f';
                continue;

            case 'n':
                bytes += '\n';
                continue;

            case 'r':
                bytes += '\r';
                continue;

            case 't':
                bytes += '\t';
                continue;

            case 'x':
                if ((index + 2) < escaped.length())
                {
                    try
                    {
                        bytes += static_cast<char>(std::stoi(escaped.substr(index + 1, 2), nullptr, 16));
                    }
                    catch (...)
                    {
                    }

                    index += 2;
                }

            default:
                break;
        }
    }

    return bytes;
}

std::string SerialAccessoryTerminalWindow::bytesToEscaped(const std::string& bytes)
{
    std::string escaped;

    for (char byte : bytes)
    {
        switch (byte)
        {
            case '"':
                escaped += "\\\"";
                continue;

            case '\\':
                escaped += "\\\\";
                continue;

            case '\b':
                escaped += "\\b";
                continue;

            case '\f':
                escaped += "\\f";
                continue;

            case '\n':
                escaped += "\\n";
                continue;

            case '\r':
                escaped += "\\r";
                continue;

            case '\t':
                escaped += "\\t";
                continue;

            default:
                break;
        }

        if ((unsigned char) byte < 0x20 || (unsigned char) byte > 0x7E)
        {
            std::ostringstream stream;
            stream << "\\x" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << byte;
            escaped += stream.str();
            continue;
        }

        escaped += byte;
    }

    return escaped;
}

std::string SerialAccessoryTerminalWindow::jsonToBytes(std::string json)
{
    if ((json.length()) < 2 || (json.front() != '"') || (json.back() != '"'))
    {
        return {};
    }
    json = json.substr(1, json.length() - 2);

    std::string bytes;

    for (size_t index = 0; index < json.length(); index++)
    {
        if (json[index] != '\\')
        {
            bytes += json[index];
            continue;
        }

        if (++index == json.length())
        {
            break;
        }

        switch (json[index])
        {
            case '"':
                bytes += '"';
                continue;

            case '\\':
                bytes += '\\';
                continue;

            case 'b':
                bytes += '\b';
                continue;

            case 'f':
                bytes += '\f';
                continue;

            case 'n':
                bytes += '\n';
                continue;

            case 'r':
                bytes += '\r';
                continue;

            case 't':
                bytes += '\t';
                continue;

            case 'u':
                if ((index + 4) < json.length())
                {
                    try
                    {
                        bytes += static_cast<char>(std::stoi(json.substr(index + 1, 4), nullptr, 16) & 0xFF);
                    }
                    catch (...)
                    {
                    }

                    index += 4;
                }

            default:
                break;
        }
    }

    return bytes;
}

std::string SerialAccessoryTerminalWindow::bytesToJson(const std::string& bytes)
{
    std::string json;

    for (char byte : bytes)
    {
        switch (byte)
        {
            case '"':
                json += "\\\"";
                continue;

            case '\\':
                json += "\\\\";
                continue;

            case '\b':
                json += "\\b";
                continue;

            case '\f':
                json += "\\f";
                continue;

            case '\n':
                json += "\\n";
                continue;

            case '\r':
                json += "\\r";
                continue;

            case '\t':
                json += "\\t";
                continue;

            default:
                break;
        }

        if ((unsigned char) byte < 0x20)
        {
            std::ostringstream stream;
            stream << "\\u" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << byte;
            json += stream.str();
            continue;
        }

        json += byte;
    }

    return "\"" + json + "\"";
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
