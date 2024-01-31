#include "ApplicationSettings.h"
#include "SendingCommandDialog.h"

SendingCommandDialog::SendingCommandDialog(const CommandMessage& command, const std::vector<ConnectionPanel*>& connectionPanels)
        : Dialog(BinaryData::progress_svg, "Sending Command " + command.json, "Retry", "Cancel", &closeWhenCompleteButton, std::numeric_limits<int>::max(), true)
{
    addAndMakeVisible(table);
    addAndMakeVisible(closeWhenCompleteButton);

    const int tagColumnWidth = UILayout::tagWidth + 5;
    table.getHeader().addColumn("", (int) ColumnIDs::tag, tagColumnWidth, tagColumnWidth, tagColumnWidth);
    table.getHeader().addColumn("", (int) ColumnIDs::titleAndError, 1);
    table.getHeader().addColumn("", (int) ColumnIDs::icon, 50, 50, 50);
    table.getHeader().setStretchToFitActive(true);
    table.setHeaderHeight(0);
    table.getViewport()->setScrollBarsShown(true, false);
    table.updateContent();
    table.setWantsKeyboardFocus(false);

    for (auto* const connectionPanel : connectionPanels)
    {
        rows.push_back({ *connectionPanel });
    }

    closeWhenCompleteButton.setClickingTogglesState(true);
    closeWhenCompleteButton.setToggleState(ApplicationSettings::getSingleton().commands.closeSendingCommandDialogWhenComplete, juce::dontSendNotification);
    closeWhenCompleteButton.onClick = [&]
    {
        ApplicationSettings::getSingleton().commands.closeSendingCommandDialogWhenComplete = closeWhenCompleteButton.getToggleState();
    };

    okCallback = [&, command]
    {
        for (auto& row : rows)
        {
            if (row.state == Row::State::complete)
            {
                continue;
            }

            row.state = Row::State::inProgress;
            row.error.clear();

            juce::Timer::callAfterDelay(sendDelay, [&, row = &row]
            {
                row->connectionPanel.sendCommands({ command }, this, [&, row](const auto& responses)
                {
                    if (responses.empty())
                    {
                        row->error = "Unable to confirm command";
                    }
                    else if (const auto error = responses[0].getError())
                    {
                        row->error = *error;
                    }
                    row->state = row->error.isEmpty() ? Row::State::complete : Row::State::failed;

                    table.updateContent();

                    if (findRow(Row::State::inProgress))
                    {
                        return;
                    }

                    if (const auto index = findRow(Row::State::failed))
                    {
                        setOkButton(true);
                        setCancelButton(true);
                        table.scrollToEnsureRowIsOnscreen(*index);
                        return;
                    }

                    okCallback = [&]
                    {
                        return true;
                    };
                    setOkButton(true, "Close");
                    setCancelButton(false);

                    startTimer(1000);
                });
            });
        }

        sendDelay = 250; // delay retry sends to improve UX

        setOkButton(false);
        setCancelButton(false);

        table.updateContent();

        return false;
    };

    okCallback();

    setSize(600, calculateHeight(0) + margin + (int) connectionPanels.size() * table.getRowHeight());
}

void SendingCommandDialog::resized()
{
    Dialog::resized();

    table.setBounds(getContentBounds(true));
}

std::optional<int> SendingCommandDialog::findRow(const Row::State state) const
{
    for (const auto [index, row] : juce::enumerate(rows))
    {
        if (row.state == state)
        {
            return (int) index;
        }
    }
    return {};
}

int SendingCommandDialog::getNumRows()
{
    return (int) rows.size();
}

void SendingCommandDialog::paintRowBackground(juce::Graphics& g, int rowNumber, int height, int, bool)
{
    if (rowNumber >= (int) rows.size())
    {
        return; // index may exceed size on Windows if display scaling >100%
    }

    g.setColour(rows[(size_t) rowNumber].connectionPanel.getTag());
    g.fillRect(0, 0, UILayout::tagWidth, height);
}

juce::Component* SendingCommandDialog::refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate)
{
    if (rowNumber >= (int) rows.size())
    {
        return existingComponentToUpdate; // index may exceed size on Windows if display scaling >100%
    }

    delete existingComponentToUpdate;

    switch ((ColumnIDs) columnID)
    {
        case ColumnIDs::tag:
            return nullptr;

        case ColumnIDs::titleAndError:
            class TitleAndError : public juce::Component
            {
            public:
                TitleAndError(const Row& row)
                        : titleLabel(row.connectionPanel.getTitle()),
                          errorLabel(row.error, UIFonts::getDefaultFont(), juce::Justification::centredRight)
                {
                    addAndMakeVisible(titleLabel);
                    addAndMakeVisible(errorLabel);
                    errorLabel.setColour(juce::Label::textColourId, UIColours::warning);
                }

                void resized() override
                {
                    auto bounds = getLocalBounds();
                    if (errorLabel.getText().isNotEmpty())
                    {
                        errorLabel.setBounds(bounds.removeFromRight((int) std::ceil(errorLabel.getTextWidth())));
                        bounds.removeFromRight(10);
                    }
                    titleLabel.setBounds(bounds);
                }

                SimpleLabel titleLabel;
                SimpleLabel errorLabel;

            private:
                JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleAndError)
            };

            return new TitleAndError(rows[(size_t) rowNumber]);

        case ColumnIDs::icon:
            switch (rows[(size_t) rowNumber].state)
            {
                case Row::State::inProgress:
                    return new Icon(BinaryData::progress_svg, "In Progress", 0.6f);

                case Row::State::failed:
                    return new Icon(BinaryData::warning_orange_svg, "Failed", 0.6f);

                case Row::State::complete:
                    return new Icon(BinaryData::tick_green_svg, "Complete", 0.6f);

                default:
                    return nullptr;
            }

        default:
            return nullptr;
    }
}

void SendingCommandDialog::timerCallback()
{
    stopTimer();

    if (ApplicationSettings::getSingleton().commands.closeSendingCommandDialogWhenComplete)
    {
        DialogQueue::getSingleton().pop();
    }
}
