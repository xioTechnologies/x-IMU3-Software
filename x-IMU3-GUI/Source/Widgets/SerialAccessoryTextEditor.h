#pragma once

#include "ApplicationSettings.h"
#include "CustomTextEditor.h"

class SerialAccessoryTextEditor : public CustomTextEditor {
public:
    SerialAccessoryTextEditor();

    bool keyPressed(const juce::KeyPress &key) override;

    int x() { return 0; }

private:
    juce::ValueTree previousData;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Previous Serial Accessory Data.xml");
    int index = 0;

    void loadPrevious();
};
