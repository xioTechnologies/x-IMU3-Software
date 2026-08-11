#include "DataLoggerSettings.h"

DataLoggerSettings::TimeUnit DataLoggerSettings::timeUnitFrom(const int timeUnit) {
    switch (static_cast<TimeUnit>(timeUnit)) {
        case TimeUnit::unlimited:
        case TimeUnit::hours:
        case TimeUnit::minutes:
        case TimeUnit::seconds:
            return static_cast<TimeUnit>(timeUnit);
    }

    return TimeUnit::unlimited;
}

std::optional<juce::RelativeTime> DataLoggerSettings::getTime() const {
    switch (timeUnit) {
        case TimeUnit::unlimited:
            return {};
        case TimeUnit::hours:
            return juce::RelativeTime::hours((double) timeValue);
        case TimeUnit::minutes:
            return juce::RelativeTime::minutes((double) timeValue);
        case TimeUnit::seconds:
            return juce::RelativeTime::seconds((double) timeValue);
    }

    return {};
}

void DataLoggerSettings::load() {
    const auto tree = juce::ValueTree::fromXml(file.loadFileAsString());

    destination = juce::File(tree.getProperty("destination", destination.getFullPathName()).toString());
    name = tree.getProperty("name", name).toString();
    nameEmpty = tree.getProperty("nameEmpty", nameEmpty);
    timeValue = tree.getProperty("timeValue", timeValue);
    timeUnit = timeUnitFrom(tree.getProperty("timeUnit", static_cast<int>(timeUnit)));
}

void DataLoggerSettings::save() const {
    juce::ValueTree tree("DataLoggerSettings");
    tree.setProperty("destination", destination.getFullPathName(), nullptr);
    tree.setProperty("name", name, nullptr);
    tree.setProperty("nameEmpty", nameEmpty, nullptr);
    tree.setProperty("timeValue", timeValue, nullptr);
    tree.setProperty("timeUnit", static_cast<int>(timeUnit), nullptr);

    std::ignore = file.create();
    std::ignore = file.replaceWithText(tree.toXmlString());
}
