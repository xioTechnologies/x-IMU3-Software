#include "ConnectionPanel/ConnectionPanel.h"
#include "LtcWindow.h"

LtcWindow::LtcWindow(const juce::ValueTree &windowLayout_, const juce::Identifier &type_, ConnectionPanel &connectionPanel_)
    : Window(windowLayout_, type_, connectionPanel_, "LTC Menu") {
    addAndMakeVisible(terminal);

    terminal.addMouseListener(this, true);

    callbackId = connectionPanel.getConnection()->addLtcCallback(callback = [&, self = SafePointer<juce::Component>(this)](auto message) {
        juce::MessageManager::callAsync([&, self, message] {
            if (self == nullptr) {
                return;
            }

            const juce::String time (message.char_array);

            juce::AttributedString line;
            line.append(time.substring(0, 8), juce::Colours::white);
            line.append(time.substring(8), (time[8] == ';') ? UIColours::special : juce::Colours::white);

            terminal.addLine(message.timestamp, line);
        });
    });

    setOpaque(true);
}

LtcWindow::~LtcWindow() {
    connectionPanel.getConnection()->removeCallback(callbackId);
}

void LtcWindow::paint(juce::Graphics &g) {
    g.fillAll(UIColours::backgroundDark);
}

void LtcWindow::resized() {
    Window::resized();

    terminal.setBounds(getContentBounds());
}

void LtcWindow::mouseDown(const juce::MouseEvent &mouseEvent) {
    if (mouseEvent.mods.isPopupMenu()) {
        getMenu().showMenuAsync({});
    }
}

juce::PopupMenu LtcWindow::getMenu() {
    juce::PopupMenu menu = Window::getMenu();
    menu.addItem("Copy to Clipboard", [&] {
        terminal.copyToClipboard();
    });
    menu.addItem("Clear", [&] {
        terminal.clearAll();
    });
    return menu;
}
