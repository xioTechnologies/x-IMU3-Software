#pragma once

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>

class Schema {
public:
    juce::String fileName;
    juce::String name;
    juce::ValueTree commands;
    juce::ValueTree settings;

    static std::vector<Schema> getAll();

    static std::optional<Schema> get(const juce::String &fileName);

    static std::optional<Schema> find(juce::String model);

    static std::unique_ptr<juce::FileChooser> add(std::function<void(const juce::File &)> callback);

    static void copyDefaultSchemas();

private:
    static juce::File getDirectory();
};
