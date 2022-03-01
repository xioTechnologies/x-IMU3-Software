#include "../ApplicationSettings.h"
#include "SendingCommandDialog.h"

SendingCommandDialog::SendingCommandDialog(const std::string& command, const std::vector<std::unique_ptr<DevicePanel>>& devicePanels) : Dialog(BinaryData::json_svg, "Sending Command " + command, "Close", "", &closeOnSuccessButton, std::numeric_limits<int>::max(), true)
{
    for (auto& panel : devicePanels)
    {
        auto deviceNameAndSerialNumber = panel->getDeviceNameAndSerialNumber();
        if (deviceNameAndSerialNumber.isNotEmpty())
        {
            deviceNameAndSerialNumber += "   ";
        }
        rows.push_back({ panel->getColourTag(), { deviceNameAndSerialNumber + panel->getConnection().getInfo()->toString() }});

        panel->getConnection().sendCommandsAsync({ command }, ApplicationSettings::getSingleton().retries, ApplicationSettings::getSingleton().timeout, [&, rowIndex = rows.size() - 1, self = SafePointer<juce::Component>(this)](const std::vector<std::string>& responses)
        {
            juce::MessageManager::callAsync([&, rowIndex, self, responses = responses]
                                            {
                                                if (self == nullptr)
                                                {
                                                    return;
                                                }

                                                rows[rowIndex].state = responses.empty() ? Row::State::failed : Row::State::complete;
                                                table.updateContent();

                                                if (ApplicationSettings::getSingleton().closeSendingCommandsOnSuccess)
                                                {
                                                    for (const auto& row : rows)
                                                    {
                                                        if (row.state != Row::State::complete)
                                                        {
                                                            return;
                                                        }
                                                    }

                                                    DialogLauncher::launchDialog(nullptr);
                                                }
                                            });
        });
    }

    addAndMakeVisible(closeOnSuccessButton);
    closeOnSuccessButton.setClickingTogglesState(true);
    closeOnSuccessButton.setToggleState(ApplicationSettings::getSingleton().closeSendingCommandsOnSuccess, juce::dontSendNotification);
    closeOnSuccessButton.onClick = [&]
    {
        ApplicationSettings::getSingleton().closeSendingCommandsOnSuccess = closeOnSuccessButton.getToggleState();
    };

    addAndMakeVisible(connectionLabel);
    addAndMakeVisible(progressLabel);

    addAndMakeVisible(table);
    const int colourTagColumnWidth = DevicePanelHeader::colourTagWidth + 5;
    table.getHeader().addColumn("", (int) ColumnIDs::colourTag, colourTagColumnWidth, colourTagColumnWidth, colourTagColumnWidth);
    table.getHeader().addColumn("", (int) ColumnIDs::connection, 1, 1);
    table.getHeader().addColumn("", (int) ColumnIDs::progress, 100, 100, 100);
    table.getHeader().setStretchToFitActive(true);
    table.setHeaderHeight(0);
    table.setColour(juce::TableListBox::backgroundColourId, UIColours::background);
    table.updateContent();

    setSize(600, calculateHeight(6));
}

void SendingCommandDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds(true);

    static constexpr int headerHeight = 30;
    bounds.removeFromTop(headerHeight);
    connectionLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnIDs::connection - 1).withHeight(headerHeight));
    progressLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnIDs::progress - 1).withHeight(headerHeight));

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

        case ColumnIDs::connection:
            return new SimpleLabel(rows[(size_t) rowNumber].connection);

        case ColumnIDs::progress:
            switch (rows[(size_t) rowNumber].state)
            {
                case Row::State::inProgress:
                    return new SimpleLabel("In Progress", UIFonts::defaultFont, juce::Justification::centred);

                case Row::State::failed:
                    return new SimpleLabel("Failed", UIFonts::defaultFont, juce::Justification::centred);

                case Row::State::complete:
                    return new SimpleLabel("Complete", UIFonts::defaultFont, juce::Justification::centred);

                default:
                    return nullptr;
            }

        default:
            return nullptr;
    }
}
