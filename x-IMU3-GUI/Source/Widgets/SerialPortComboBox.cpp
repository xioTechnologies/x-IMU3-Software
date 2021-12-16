#include "SerialDiscovery.hpp"
#include "SerialPortComboBox.h"

SerialPortComboBox::SerialPortComboBox()
{
    updateSerialPorts();
    setSelectedItemIndex(0);
}

void SerialPortComboBox::mouseDown(const juce::MouseEvent& e)
{
    updateSerialPorts();
    juce::ComboBox::mouseDown(e);
}

const std::string& SerialPortComboBox::getSelectedPortName() const
{
    if (previousPortNames.size() > 0 && getSelectedId() > 0)
    {
        return previousPortNames[(size_t) (getSelectedId() - 1)];
    }

    jassertfalse;
    static const std::string fallback;
    return fallback;
}

void SerialPortComboBox::updateSerialPorts()
{
    if (const auto portNames = ximu3::SerialDiscovery::getAvailablePorts(); previousPortNames != portNames)
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
