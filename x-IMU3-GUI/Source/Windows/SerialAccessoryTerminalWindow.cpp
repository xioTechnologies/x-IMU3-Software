#include "ConnectionPanel/ConnectionPanel.h"
#include "Dialogs/SendingCommandDialog.h"
#include "EscapedStrings.h"
#include "SerialAccessoryTerminalWindow.h"

SerialAccessoryTerminalWindow::SerialAccessoryTerminalWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_)
    : Window(windowLayout_, type_, connectionPanel_, "Serial Accessory Terminal Menu")
{
    addAndMakeVisible(terminal);
    addAndMakeVisible(textEditor);

    terminal.addMouseListener(this, true);

    textEditor.onReturnKey = [&]
    {
        std::string command = "{\"accessory\":" + EscapedStrings::bytesToJson(EscapedStrings::printableToBytes(textEditor.getText().toStdString())) + "}";

        DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(command, std::vector<ConnectionPanel*> { &connectionPanel }));

        terminal.addTx(EscapedStrings::bytesToPrintable(EscapedStrings::jsonToBytes(textEditor.getText().toStdString())));
    };

    callbackId = connectionPanel.getConnection()->addSerialAccessoryCallback(callback = [&, self = SafePointer<juce::Component>(this)](auto message)
    {
        juce::MessageManager::callAsync([&, self, message]
        {
            if (self == nullptr)
            {
                return;
            }

            terminal.addRx(message.timestamp, EscapedStrings::bytesToPrintable({ message.char_array, (unsigned int) message.number_of_bytes }));
        });
    });

    setOpaque(true);
}

SerialAccessoryTerminalWindow::~SerialAccessoryTerminalWindow()
{
    connectionPanel.getConnection()->removeCallback(callbackId);
}

void SerialAccessoryTerminalWindow::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::backgroundDark);
}

void SerialAccessoryTerminalWindow::resized()
{
    Window::resized();

    auto bounds = getContentBounds();
    textEditor.setBounds(bounds.removeFromBottom(UILayout::textComponentHeight));
    terminal.setBounds(bounds);
}

void SerialAccessoryTerminalWindow::mouseDown(const juce::MouseEvent& mouseEvent)
{
    if (mouseEvent.mods.isPopupMenu())
    {
        getMenu().showMenuAsync({});
    }
}

juce::PopupMenu SerialAccessoryTerminalWindow::getMenu()
{
    juce::PopupMenu menu = Window::getMenu();
    menu.addItem("Copy to Clipboard", [&]
    {
        terminal.copyToClipboard();
    });
    menu.addItem("Clear", [&]
    {
        terminal.clearAll();
    });
    return menu;
}
