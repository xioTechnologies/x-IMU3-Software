#include "DevicePanel/DevicePanel.h"
#include "SerialAccessoryTerminalWindow.h"

SerialAccessoryTerminalWindow::SerialAccessoryTerminalWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_) : Window(windowLayout, type, devicePanel_)
{
    addAndMakeVisible(terminalFeed);

    addAndMakeVisible(sendValue);
    sendValue.setEditableText(true);

    loadRecentSends();

    addAndMakeVisible(sendButton);
    sendButton.addShortcut(juce::KeyPress(juce::KeyPress::returnKey));
    sendButton.onClick = [this]
    {
        devicePanel.sendCommands({ CommandMessage("{\"accessory\":" + sendValue.getText().quoted().toStdString() + "}") });
        terminalFeed.add("TX", sendValue.getText(), UIColours::grey);

        if (sendValue.getText().isEmpty())
        {
            return;
        }

        for (const auto recentSend : recentSends)
        {
            if (recentSend.getProperty("send") == sendValue.getText())
            {
                recentSends.removeChild(recentSend, nullptr);
                break;
            }
        }

        while (recentSends.getNumChildren() > 9)
        {
            recentSends.removeChild(recentSends.getChild(recentSends.getNumChildren() - 1), nullptr);
        }

        recentSends.addChild({ "Send", {{ "send", sendValue.getText() }}}, 0, nullptr);
        file.replaceWithText(recentSends.toXmlString());

        loadRecentSends();
    };

    callbackID = devicePanel.getConnection().addSerialAccessoryCallback(callback = [&, self = SafePointer<juce::Component>(this)](auto message)
    {
        juce::MessageManager::callAsync([&, self, message]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            terminalFeed.add(message.timestamp, message.char_array, juce::Colours::white);
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

    terminalFeed.setBounds(bounds);
}

void SerialAccessoryTerminalWindow::loadRecentSends()
{
    recentSends = juce::ValueTree::fromXml(file.loadFileAsString());
    if (recentSends.isValid() == false)
    {
        recentSends = juce::ValueTree("RecentSends");
    }

    sendValue.clear(juce::dontSendNotification);
    for (const auto recentSend : recentSends)
    {
        sendValue.addItem(recentSend.getProperty("send"), sendValue.getNumItems() + 1);
    }

    sendValue.setText(sendValue.getNumItems() > 0 ? sendValue.getItemText(0) : "Hello, World!", juce::dontSendNotification);
}
