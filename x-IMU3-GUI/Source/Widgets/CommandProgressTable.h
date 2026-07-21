#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class CommandProgressTable : public juce::Component, private juce::TableListBoxModel {
public:
    enum class Status {
        inProgress,
        complete,
        failed,
    };

    CommandProgressTable();

    void resized() override;

    int getIdealHeight() const;

    void addRow(const juce::Colour &colourTag, const juce::String &connection);

    void setInProgress(int rowIndex, std::optional<int> percentage);

    void setComplete(int rowIndex, const juce::String &value);

    void setFailed(int rowIndex, const juce::String &error);

    Status getStatus() const;

    Status getStatus(const int rowIndex) const;

private:
    struct Row {
        juce::Colour colourTag;
        juce::String connection;
        Status status;
        std::optional<int> percentage;
        juce::String response;
    };

    std::vector<Row> rows;

    enum class ColumnId {
        tag = 1,
        body,
        icon,
    };

    juce::TableListBox table{"", this};

    Row *getRow(int index);

    const Row *getRow(int index) const;

    int getNumRows() override;

    void paintRowBackground(juce::Graphics &, int rowIndex, int, int height, bool) override;

    void paintCell(juce::Graphics &, int, int, int, int, bool) override;

    juce::Component *refreshComponentForCell(int rowIndex, int columnId, bool, juce::Component *existingComponentToUpdate) override;
};
