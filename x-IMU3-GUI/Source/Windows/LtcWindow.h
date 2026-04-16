#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/Terminal.h"
#include "Window.h"

class LtcWindow : public Window {
public:
    LtcWindow(const juce::ValueTree &windowLayout, const juce::Identifier &type, ConnectionPanel &connectionPanel_);

    ~LtcWindow() override;

    void paint(juce::Graphics &g) override;

    void resized() override;

    void mouseDown(const juce::MouseEvent &mouseEvent) override;

private:
    Terminal terminal;

    std::function<void(ximu3::XIMU3_LtcMessage)> callback;
    uint64_t callbackId;

    juce::PopupMenu getMenu() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LtcWindow)
};
