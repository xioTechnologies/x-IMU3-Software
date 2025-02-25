#include "ConnectionPanel/ConnectionPanel.h"
#include "Dialogs/SendingCommandDialog.h"
#include "EscapedStrings.h"
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
        CommandMessage commandMessage("{\"accessory\":" + EscapedStrings::bytesToJson(EscapedStrings::printableToBytes(sendValue.getText().toStdString())) + "}");
        if (sendValue.getText().isEmpty() || commandMessage.json.empty())
        {
            serialAccessoryTerminal.addError(sendValue.getText());
            return;
        }

        DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(commandMessage, std::vector<ConnectionPanel*> { &connectionPanel }));

        serialAccessoryTerminal.addTX(EscapedStrings::bytesToPrintable(EscapedStrings::jsonToBytes(commandMessage.value)));

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

            serialAccessoryTerminal.addRX(message.timestamp, EscapedStrings::bytesToPrintable({ message.char_array, (unsigned int) message.number_of_bytes }));
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

    sendValue.setText(sendValue.getNumItems() > 0 ? sendValue.getItemText(0) : R"(Use escape sequences "\x00" to "\xFF" to send any byte value)", juce::dontSendNotification);
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
