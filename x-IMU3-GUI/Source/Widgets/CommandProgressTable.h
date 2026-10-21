#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "CommandProgressRow.h"

class CommandProgressTable : public juce::Viewport {
public:
    CommandProgressTable();

    void paint(juce::Graphics &g) override;

    void resized() override;

    int getIdealHeight() const;

    void addRow(const juce::Colour &colourTag, const juce::String &connection);

    void setInProgress(int rowIndex, std::optional<int> percentage);

    void setComplete(int rowIndex, const juce::String &value);

    void setFailed(int rowIndex, const juce::String &error);

    CommandProgressRow::Status getStatus() const;

    CommandProgressRow::Status getStatus(const int rowIndex) const;

private:
    static constexpr int rowHeight = 22;

    juce::Component content;
    std::vector<std::unique_ptr<CommandProgressRow>> rows;

    CommandProgressRow *getRow(int index) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandProgressTable)
};
