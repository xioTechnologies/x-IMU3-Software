#pragma once

#include "CustomComboBox.h"
#include <juce_gui_basics/juce_gui_basics.h>

class PortNameComboBox : public CustomComboBox
{
public:
    PortNameComboBox();

    void mouseDown(const juce::MouseEvent& e) override;

    const std::string& getSelectedPortName() const;

private:
    std::vector<std::string> previousPortNames;

    void updatePorts();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PortNameComboBox)
};
