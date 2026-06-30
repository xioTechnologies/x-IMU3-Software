#pragma once

#include "../DeviceSettingX.h"

class SettingValue : public juce::Component {
public:
    SettingValue(DeviceSettingX &setting_) : setting(setting_) {
    }

    virtual void refresh() = 0;

    virtual std::optional<std::string> getValue() const = 0;

    static std::unique_ptr<SettingValue> create(DeviceSettingX &setting);

protected:
    DeviceSettingX &setting;
};
