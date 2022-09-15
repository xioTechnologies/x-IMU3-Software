#include "PortNameComboBox.h"
#include "Ximu3.hpp"

PortNameComboBox::PortNameComboBox()
{
    updatePorts();
    setSelectedItemIndex(0);
}

void PortNameComboBox::mouseDown(const juce::MouseEvent& e)
{
    updatePorts();
    juce::ComboBox::mouseDown(e);
}

const std::string& PortNameComboBox::getSelectedPortName() const
{
    if (previousPortNames.size() > 0 && getSelectedId() > 0)
    {
        return previousPortNames[(size_t) (getSelectedId() - 1)];
    }

    jassertfalse;
    static const std::string fallback;
    return fallback;
}

void PortNameComboBox::updatePorts()
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
