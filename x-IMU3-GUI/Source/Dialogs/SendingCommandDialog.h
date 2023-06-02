#pragma once

#include "../CommandMessage.h"
#include "../DevicePanel/DevicePanel.h"
#include "../Widgets/CustomToggleButton.h"
#include "../Widgets/SimpleLabel.h"
#include "Dialog.h"

class SendingCommandDialog : public Dialog,
                             private juce::TableListBoxModel,
                             private juce::Timer
{
public:
    SendingCommandDialog(const CommandMessage& command, const std::vector<DevicePanel*>& devicePanels);

    void resized() override;

private:
    enum class ColumnIDs
    {
        tag = 1,
        device,
        connection,
        complete,
    };

    struct Row
    {
        enum class State
        {
            inProgress,
            failed,
            complete,
        };

        DevicePanel& devicePanel;
        State state = State::inProgress;
    };

    SimpleLabel deviceLabel { "Device" };
    SimpleLabel connectionLabel { "Connection" };
    SimpleLabel completeLabel { "Complete", UIFonts::getDefaultFont(), juce::Justification::centred };
    juce::TableListBox table { "", this };
    std::vector<Row> rows;

    CustomToggleButton closeWhenCompleteButton { "Close When Complete" };

    int getNumRows() override;

    void paintRowBackground(juce::Graphics&, int, int, int, bool) override;

    void paintCell(juce::Graphics&, int, int, int, int, bool) override
    {
    }

    juce::Component* refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate) override;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendingCommandDialog)
};
