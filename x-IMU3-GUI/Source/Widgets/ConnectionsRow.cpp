#include "ConnectionsRow.h"

ConnectionsRow::ConnectionsRow(std::function<void(bool)> onSelectedChanged_)
    : onSelectedChanged(std::move(onSelectedChanged_)) {
    addAndMakeVisible(toggle);
    addAndMakeVisible(descriptorLabel);
    addAndMakeVisible(connectionLabel);
    addAndMakeVisible(rssiIcon);
    addAndMakeVisible(batteryIcon);

    toggle.onClick = [&] {
        data.selected = toggle.getToggleState();
        juce::NullCheckedInvocation::invoke(onSelectedChanged, data.selected);
    };
}

void ConnectionsRow::resized() {
    auto bounds = getLocalBounds();
    toggle.setBounds(bounds);

    bounds.removeFromRight(5);
    batteryIcon.setBounds(bounds.removeFromRight(25));
    rssiIcon.setBounds(bounds.removeFromRight(25));

    bounds.removeFromLeft(toggleWidth);
    descriptorLabel.setBounds(bounds.removeFromLeft(bounds.getWidth() / 3));
    connectionLabel.setBounds(bounds);
}

const ConnectionsRow::Data &ConnectionsRow::getData() const {
    return data;
}

void ConnectionsRow::setData(const Data &data_) {
    data = data_;

    toggle.setToggleState(data.selected, juce::dontSendNotification);

    descriptorLabel.setText(data.descriptor);
    connectionLabel.setText(data.config->toString());

    if (data.rssiPercentage.has_value() && (data.rssiPercentage != -1)) {
        rssiIcon.update(*data.rssiPercentage);
    } else {
        rssiIcon.unavailable();
    }

    if (data.batteryPercentage.has_value() && (data.batteryPercentage != -1)) {
        batteryIcon.update(*data.batteryPercentage, *data.batteryStatus);
    } else {
        batteryIcon.unavailable();
    }

    setEnabled(data.unavailable == false);
}
