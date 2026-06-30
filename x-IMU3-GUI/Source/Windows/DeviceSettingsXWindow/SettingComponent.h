#pragma once

#include "Widgets/SimpleLabel.h"
#include "Values/SettingValue.h"

class SettingComponent : public juce::Component {
public:
    SettingComponent(DeviceSettingX &setting_) : setting(setting_) {
        addAndMakeVisible(nameLabel);
        addAndMakeVisible(statusLabel);
        addAndMakeVisible(value.get());

        refresh();
    }

    void resized() override {
        auto r = getLocalBounds();
        nameLabel.setBounds(r.removeFromLeft(r.getWidth() / 3));
        r.removeFromLeft(5);
        value->setBounds(r.removeFromLeft(r.getWidth() / 2));
        r.removeFromLeft(5);
        statusLabel.setBounds(r);
    }

    void refresh() {
        switch (setting.status) {
            case DeviceSettingX::Status::unknown:
                statusLabel.setText("Unknown");
                break;

            case DeviceSettingX::Status::noResponse:
                statusLabel.setText("No Response");
                break;

            case DeviceSettingX::Status::errorResponse:
                statusLabel.setText("Error Response");
                break;

            case DeviceSettingX::Status::invalidResponse:
                statusLabel.setText("Invalid Response");
                break;

            case DeviceSettingX::Status::confirmed:
                statusLabel.setText("Confirmed");

                value->refresh();
                break;
        }
    }

    std::optional<std::string> getValue() const {
        return value->getValue();
    }

private:
    DeviceSettingX &setting;
    SimpleLabel nameLabel{setting.name};
    const std::unique_ptr<SettingValue> value = SettingValue::create(setting);
    SimpleLabel statusLabel;
};
