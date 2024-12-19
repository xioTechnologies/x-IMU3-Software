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
        const auto json = "{\"accessory\":\"" + toUnquotedJsonString(sendValue.getText().toStdString()) + "\"}";
        if (sendValue.getText().isEmpty() || CommandMessage(json).json.empty())
        {
            serialAccessoryTerminal.addError(sendValue.getText());
            return;
        }

        DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(CommandMessage{json}, std::vector<ConnectionPanel*> { &connectionPanel }));

        serialAccessoryTerminal.addTX(fromUnquotedJsonString(ximu3::XIMU3_command_message_parse(json.data()).value));

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

            serialAccessoryTerminal.addRX(message.timestamp, juce::String::createStringFromData(message.char_array, (int) message.number_of_bytes));
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

std::string SerialAccessoryTerminalWindow::toUnquotedJsonString(const std::string& input)
{
    std::string output;
    std::string::const_iterator searchStart(input.begin());
    std::smatch match;
    while (std::regex_search(searchStart, input.end(), match, std::regex(R"(\\x([0-9A-Fa-f]{2}))"))) {
        output.append(searchStart, match.prefix().second);

        const auto value = std::stoi(match[1].str(), nullptr, 16);
        if (value < 0x20)
        {
            std::ostringstream stream;
            stream << "\\u00" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << value;
            output += stream.str();
        }
        else if (value == 0x22)
        {
            output += "\\\"";
        }
        else if (value == 0x5C)
        {
            output += "\\\\";
        }
        else
        {
            output += static_cast<char>(value);
        }

        searchStart = match.suffix().first;
    }
    output += {searchStart, input.end()};
    return output;
}

std::string SerialAccessoryTerminalWindow::fromUnquotedJsonString(const std::string& input)
{
    std::string output;
    for (unsigned char character : input)
    {
        if (character >= 0x7F)
        {
            std::ostringstream stream;
            stream << "\\x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int) character;
            output += stream.str();
        }
        else
        {
            output += character;
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
