#pragma once

#include "ApplicationSettings.h"
#include <juce_gui_basics/juce_gui_basics.h>

struct DataLoggerSettings {
    enum class TimeUnit {
        unlimited,
        hours,
        minutes,
        seconds,
    };

    juce::File file = ApplicationSettings::getDirectory().getChildFile("Data Logger Settings.xml");

    juce::File destination = ApplicationSettings::getDirectory().getChildFile("Data Logger");
    juce::String name;
    bool nameEmpty = true;
    float timeValue = 10.0f;
    TimeUnit timeUnit = TimeUnit::unlimited;

    static TimeUnit timeUnitFrom(const int timeUnit);

    std::optional<juce::RelativeTime> getTime() const;

    void load();

    void save() const;
};
