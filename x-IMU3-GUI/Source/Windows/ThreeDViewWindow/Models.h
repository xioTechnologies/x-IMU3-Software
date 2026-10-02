#pragma once

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

class Models {
public:
    static std::vector<juce::File> getAll();

    static std::optional<juce::File> get(const juce::String &fileName);

    static std::optional<juce::File> find(juce::String model);

    static std::unique_ptr<juce::FileChooser> add(std::function<void(const juce::File &)> callback);

    static void copyDefaultModels();

private:
    static juce::File getDirectory();
};
