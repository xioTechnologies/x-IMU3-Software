#include "../Helpers.h"
#include "DevicePanel/DevicePanel.h"
#include "SerialAccessoryTerminalWindow.h"

SerialAccessoryTerminalWindow::SerialAccessoryTerminalWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, DevicePanel& devicePanel_) : Window(windowLayout_, type_, devicePanel_)
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

        serialAccessoryTerminal.add(uint64_t(-1), Helpers::removeEscapeCharacters(sendValue.getText()));

        devicePanel.sendCommands({ CommandMessage("accessory", Helpers::removeEscapeCharacters(sendValue.getText())) }, this, [&](const auto& responses, const auto&)
        {
            sendValue.setEnabled(true);
            sendButton.setEnabled(true);
            sendButton.setToggleState(responses.empty(), juce::dontSendNotification);
        });

        for (const auto send : sendHistory)
        {
            if (send.getProperty("send") == sendValue.getText())
            {
                sendHistory.removeChild(send, nullptr);
                break;
            }
        }

        while (sendHistory.getNumChildren() > 9)
        {
            sendHistory.removeChild(sendHistory.getChild(sendHistory.getNumChildren() - 1), nullptr);
        }

        sendHistory.addChild({ "Send", {{ "send", sendValue.getText() }}}, 0, nullptr);
        file.replaceWithText(sendHistory.toXmlString());

        loadSendHistory();
    };

    callbackID = devicePanel.getConnection().addSerialAccessoryCallback(callback = [&, self = SafePointer<juce::Component>(this)](auto message)
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
    devicePanel.getConnection().removeCallback(callbackID);
}

void SerialAccessoryTerminalWindow::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::background);
    Window::paint(g);
}

void SerialAccessoryTerminalWindow::resized()
{
    Window::resized();
    auto bounds = getContentBounds();

    auto sendCommandBounds = bounds.removeFromBottom(UILayout::textComponentHeight + 2 * widgetMargin);
    sendButton.setBounds(sendCommandBounds.removeFromRight(45).reduced(widgetMargin));
    sendValue.setBounds(sendCommandBounds.reduced(widgetMargin));

    serialAccessoryTerminal.setBounds(bounds);
}

void SerialAccessoryTerminalWindow::loadSendHistory()
{
    sendHistory = juce::ValueTree::fromXml(file.loadFileAsString());
    if (sendHistory.isValid() == false)
    {
        sendHistory = juce::ValueTree("SendHistory");
    }

    sendValue.clear(juce::dontSendNotification);
    for (const auto send : sendHistory)
    {
        sendValue.addItem(send.getProperty("send"), sendValue.getNumItems() + 1);
    }

    sendValue.setText(sendValue.getNumItems() > 0 ? sendValue.getItemText(0) : "Hello, World!", juce::dontSendNotification);
}
