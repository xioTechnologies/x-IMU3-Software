#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "CustomToggleButton.h"
#include "Icon.h"
#include "SimpleLabel.h"
#include "Ximu3.hpp"

class ConnectionsRow : public juce::Component {
public:
    struct Data {
        bool selected;
        juce::String descriptor;
        std::shared_ptr<ximu3::ConnectionConfig> config;
        std::optional<int> rssiPercentage;
        std::optional<int> batteryPercentage;
        std::optional<ximu3::XIMU3_ChargingStatus> batteryStatus;
        bool unavailable;

        bool operator==(const Data &other) const {
            return config->toString() == other.config->toString();
        }
    };

    static constexpr int toggleWidth = 40;

    explicit ConnectionsRow(std::function<void(bool)> onSelectedChanged_);

    void resized() override;

    const Data &getData() const;

    void setData(const Data &data_);

private:
    const std::function<void(bool)> onSelectedChanged;

    Data data;

    CustomToggleButton toggle{""};
    SimpleLabel descriptorLabel;
    SimpleLabel connectionLabel;
    RssiIcon rssiIcon{0.65f};
    BatteryIcon batteryIcon{0.65f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectionsRow)
};
