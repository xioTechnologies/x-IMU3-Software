#pragma once

#include "../Widgets/CustomToggleButton.h"
#include "../Widgets/SimpleLabel.h"
#include "ConnectionInfo.hpp"
#include <juce_gui_basics/juce_gui_basics.h>
#include <variant>
#include "Ximu3.hpp"

class DiscoveredDevicesTable : public juce::Component,
                               private juce::TableListBoxModel
{
public:
    struct DiscoveredDevice
    {
        bool selected = false;
        juce::String deviceNameAndSerialNumber;
        ximu3::XIMU3_ConnectionType connectionType;
        std::unique_ptr<ximu3::ConnectionInfo> connectionInfo;
    };

    DiscoveredDevicesTable(std::vector<DiscoveredDevice>& discoveredDevices_);

    void paint(juce::Graphics& g) override;

    void resized() override;

    void update();

    std::function<void()> onSelectionChanged;

private:
    static constexpr int selectionColumnID = 1;
    static constexpr int nameAndSerialNumberColumnID = 2;
    static constexpr int infoColumnID = 3;

    std::vector<DiscoveredDevice>& discoveredDevices;

    CustomToggleButton buttonSelectAll { "" };
    SimpleLabel labelSelectAll { "Select All" };
    SimpleLabel numConnectionsFoundLabel;
    juce::TableListBox table { "", this };

    void selectionChanged();

    int getNumRows() override;

    void paintRowBackground(juce::Graphics& g, int rowNumber, int, int, bool) override;

    void paintCell(juce::Graphics&, int, int, int, int, bool) override
    {
    }

    juce::Component* refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate) override;

    void cellClicked(int rowNumber, int, const juce::MouseEvent&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiscoveredDevicesTable)
};
