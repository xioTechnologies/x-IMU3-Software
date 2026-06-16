#include "PortNameComboBox.h"
#include "Ximu3.hpp"

PortNameComboBox::PortNameComboBox() : CustomComboBox("No Ports Found") {
    updatePorts();
    setSelectedItemIndex(0);
}

void PortNameComboBox::mouseDown(const juce::MouseEvent &e) {
    updatePorts();
    juce::ComboBox::mouseDown(e);
}

std::string PortNameComboBox::getSelectedPortName() const {
    try {
        return previousPortNames.at((size_t) (getSelectedId() - 1));
    } catch (...) {
        return {};
    }
}

void PortNameComboBox::updatePorts() {
    if (const auto portNames = ximu3::PortScanner::getPortNames(); previousPortNames != portNames) {
        previousPortNames = portNames;

        const auto id = getSelectedId();

        clear();
        for (auto &portName: portNames) {
            addItem(portName, getNumItems() + 1);
        }

        setSelectedId(id);
    }
}
