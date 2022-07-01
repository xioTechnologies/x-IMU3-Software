#include "../ApplicationSettings.h"
#include "SendingCommandDialog.h"

SendingCommandDialog::SendingCommandDialog(const CommandMessage& command, const std::vector<DevicePanel*>& devicePanels) : Dialog(BinaryData::progress_svg, "Sending Command " + command.json, "Close", "", &closeWhenCompleteButton, std::numeric_limits<int>::max(), true)
{
    for (auto* const devicePanel : devicePanels)
    {
        rows.push_back({ devicePanel->getColourTag(), devicePanel->getDeviceDescriptor(), devicePanel->getConnection().getInfo()->toString() });

        devicePanel->sendCommands({ command }, this, [&, rowIndex = rows.size() - 1](const auto&, const auto& failedCommands)
        {
            rows[rowIndex].state = (failedCommands.empty() == false) ? Row::State::failed : Row::State::complete;
            table.updateContent();

            if (ApplicationSettings::getSingleton().closeSendingCommandDialogWhenComplete)
            {
                for (const auto& row : rows)
                {
                    if (row.state != Row::State::complete)
                    {
                        return;
                    }
                }

                startTimer(1000);
            }
        });
    }

    addAndMakeVisible(closeWhenCompleteButton);
    closeWhenCompleteButton.setClickingTogglesState(true);
    closeWhenCompleteButton.setToggleState(ApplicationSettings::getSingleton().closeSendingCommandDialogWhenComplete, juce::dontSendNotification);
    closeWhenCompleteButton.onClick = [&]
    {
        ApplicationSettings::getSingleton().closeSendingCommandDialogWhenComplete = closeWhenCompleteButton.getToggleState();
    };

    addAndMakeVisible(deviceLabel);
    addAndMakeVisible(connectionLabel);
    addAndMakeVisible(completeLabel);

    addAndMakeVisible(table);
    const int colourTagColumnWidth = DevicePanelHeader::colourTagWidth + 5;
    table.getHeader().addColumn("", (int) ColumnIDs::colourTag, colourTagColumnWidth, colourTagColumnWidth, colourTagColumnWidth);
    table.getHeader().addColumn("", (int) ColumnIDs::device, 1);
    table.getHeader().addColumn("", (int) ColumnIDs::connection, 1);
    table.getHeader().addColumn("", (int) ColumnIDs::complete, 70, 70, 70);
    table.getHeader().setStretchToFitActive(true);
    table.setHeaderHeight(0);
    table.getViewport()->setScrollBarsShown(true, false);
    table.setColour(juce::TableListBox::backgroundColourId, UIColours::background);
    table.updateContent();
    table.setWantsKeyboardFocus(false);

    setSize(600, calculateHeight(6));
}

void SendingCommandDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds(true);

    static constexpr int headerHeight = 30;
    bounds.removeFromTop(headerHeight);
    deviceLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnIDs::device - 1).withHeight(headerHeight));
    connectionLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnIDs::connection - 1).withHeight(headerHeight));
    completeLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnIDs::complete - 1).withHeight(headerHeight));

    table.setBounds(bounds);
}

int SendingCommandDialog::getNumRows()
{
    return (int) rows.size();
}

void SendingCommandDialog::paintRowBackground(juce::Graphics& g, int rowNumber, int height, int, bool)
{
    g.setColour(rows[(size_t) rowNumber].colourTag);
    g.fillRect(0, 0, DevicePanelHeader::colourTagWidth, height);
}

juce::Component* SendingCommandDialog::refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate)
{
    delete existingComponentToUpdate;

    switch ((ColumnIDs) columnID)
    {
        case ColumnIDs::colourTag:
            return nullptr;

        case ColumnIDs::device:
            return new SimpleLabel(rows[(size_t) rowNumber].device);

        case ColumnIDs::connection:
            return new SimpleLabel(rows[(size_t) rowNumber].connection);

        case ColumnIDs::complete:
            switch (rows[(size_t) rowNumber].state)
            {
                case Row::State::inProgress:
                    return new Icon(BinaryData::progress_svg, 0.6f, "In Progress");

                case Row::State::failed:
                    return new Icon(BinaryData::warning_orange_svg, 0.6f, "Failed");

                case Row::State::complete:
                    return new Icon(BinaryData::tick_green_svg, 0.6f, "Complete");

                default:
                    return nullptr;
            }

        default:
            return nullptr;
    }
}

void SendingCommandDialog::timerCallback()
{
    DialogLauncher::launchDialog(nullptr);
}
