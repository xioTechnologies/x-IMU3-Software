#pragma once

#include "../Schema/SettingX.h"

class ValueBase : public juce::Component {
public:
    ValueBase(SettingX &setting_) : setting(setting_) {
    }

    virtual void refresh() = 0;

protected:
    SettingX &setting;
};
