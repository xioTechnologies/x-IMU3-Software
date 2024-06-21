#pragma once

#include "CommandMessage.h"
#include "ConnectionPanel/ConnectionPanel.h"
#include "Dialog.h"
#include "Widgets/CustomToggleButton.h"

class SendingCommandDialog : public Dialog,
                             private juce::TableListBoxModel,
                             private juce::Timer
{
public:
    SendingCommandDialog(const CommandMessage& command, const std::vector<ConnectionPanel*>& connectionPanels);

    void resized() override;

private:
    enum class ColumnIDs
    {
        tag = 1,
        titleAndResponse,
        icon,
    };

    struct Row
    {
        enum class State
        {
            inProgress,
            failed,
            complete,
        };

        ConnectionPanel& connectionPanel;
        State state = State::inProgress;
        juce::String response {};
    };

    juce::TableListBox table { "", this };
    std::vector<Row> rows;

    CustomToggleButton closeWhenCompleteButton { "Close When Complete" };

    int sendDelay = 0;

    std::optional<int> findRow(const Row::State state) const;

    int getNumRows() override;

    void paintRowBackground(juce::Graphics&, int, int, int, bool) override;

    void paintCell(juce::Graphics&, int, int, int, int, bool) override
    {
    }

    juce::Component* refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate) override;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendingCommandDialog)
};
