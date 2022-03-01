#pragma once

#include "../DevicePanel/DevicePanel.h"
#include "Dialog.h"
#include "../Widgets/CustomToggleButton.h"
#include "../Widgets/SimpleLabel.h"

class SendingCommandDialog : public Dialog,
                             private juce::TableListBoxModel
{
public:
    SendingCommandDialog(const std::string& command, const std::vector<std::unique_ptr<DevicePanel>>& devicePanels);

    void resized() override;

private:
    enum class ColumnIDs
    {
        colourTag = 1,
        connection = 2,
        progress = 3,
    };

    struct Row
    {
        enum class State
        {
            inProgress,
            failed,
            complete
        };

        juce::Colour colourTag;
        juce::String connection;
        State state = State::inProgress;
    };

    std::vector<Row> rows;

    CustomToggleButton closeOnSuccessButton { "Close On Success" };

    SimpleLabel connectionLabel { "Connection" };
    SimpleLabel progressLabel { "Progress", UIFonts::defaultFont, juce::Justification::centred };
    juce::TableListBox table { "", this };

    int getNumRows() override;

    void paintRowBackground(juce::Graphics&, int, int, int, bool) override;

    void paintCell(juce::Graphics&, int, int, int, int, bool) override
    {
    }

    juce::Component* refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendingCommandDialog)
};
