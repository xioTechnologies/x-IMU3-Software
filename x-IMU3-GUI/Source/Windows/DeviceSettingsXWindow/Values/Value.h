#pragma once

#include "../Schema/SettingX.h"

class Value : public juce::Component {
public:
    Value(SettingX &setting_) : setting(setting_) {
    }

    virtual void refresh() = 0;

protected:
    SettingX &setting;
};
