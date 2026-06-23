#pragma once

#include "../Schema/SettingX.h"

class SettingValue : public juce::Component {
public:
    SettingValue(SettingX &setting_) : setting(setting_) {
    }

    virtual void refresh() = 0;

    static std::unique_ptr<SettingValue> create(SettingX &setting);

protected:
    SettingX &setting;
};
