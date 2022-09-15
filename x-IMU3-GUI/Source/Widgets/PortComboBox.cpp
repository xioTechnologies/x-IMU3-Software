#include "PortComboBox.h"
#include "Ximu3.hpp"

PortComboBox::PortComboBox()
{
    updatePorts();
    setSelectedItemIndex(0);
}

void PortComboBox::mouseDown(const juce::MouseEvent& e)
{
    updatePorts();
    juce::ComboBox::mouseDown(e);
}

const std::string& PortComboBox::getSelectedPortName() const
{
    if (previousPortNames.size() > 0 && getSelectedId() > 0)
    {
        return previousPortNames[(size_t) (getSelectedId() - 1)];
    }

    jassertfalse;
    static const std::string fallback;
    return fallback;
}

void PortComboBox::updatePorts()
{
    if (const auto portNames = ximu3::PortScanner::getPortNames(); previousPortNames != portNames)
    {
        previousPortNames = portNames;

        const auto id = getSelectedId();

        clear();
        for (auto& portName : portNames)
        {
            addItem(portName, getNumItems() + 1);
        }

        setSelectedId(id);
    }
}
