#include "ApplicationSettings.h"
#include "SendingCommandDialog.h"
#include "Widgets/SimpleLabel.h"

SendingCommandDialog::SendingCommandDialog(const std::string &command, const std::vector<ConnectionPanel *> &connectionPanels)
    : Dialog(BinaryData::progress_svg, "Sending Command " + replaceInvalidCharacters(command), "Retry", "Cancel", &closeWhenCompleteButton, 175, true) {
    addAndMakeVisible(table);
    addAndMakeVisible(closeWhenCompleteButton);

    const int tagColumnWidth = UILayout::tagWidth + 5;
    table.getHeader().addColumn("", (int) ColumnIds::tag, tagColumnWidth, tagColumnWidth, tagColumnWidth);
    table.getHeader().addColumn("", (int) ColumnIds::headingAndResponse, 1);
    table.getHeader().addColumn("", (int) ColumnIds::icon, 50, 50, 50);
    table.getHeader().setStretchToFitActive(true);
    table.setHeaderHeight(0);
    table.getViewport()->setScrollBarsShown(true, false);
    table.updateContent();
    table.setWantsKeyboardFocus(false);

    for (auto *const connectionPanel: connectionPanels) {
        rows.push_back({*connectionPanel});
    }

    closeWhenCompleteButton.setClickingTogglesState(true);
    closeWhenCompleteButton.setToggleState(ApplicationSettings::getSingleton().commands.closeSendingCommandDialogWhenComplete, juce::dontSendNotification);
    closeWhenCompleteButton.onClick = [&] {
        ApplicationSettings::getSingleton().commands.closeSendingCommandDialogWhenComplete = closeWhenCompleteButton.getToggleState();
    };

    okCallback = [&, command] {
        for (auto &row: rows) {
            if (row.state == Row::State::complete) {
                continue;
            }

            row.state = Row::State::inProgress;
            row.response.clear();

            juce::Timer::callAfterDelay(sendDelay, [&, row_ = &row] {
                row_->connectionPanel.sendCommands({command}, this, [&, row_](const auto &responses) {
                    if (responses.front().has_value() == false) {
                        row_->state = Row::State::failed;
                        row_->response = "No response";
                    } else if (const auto error = responses.front()->error) {
                        row_->state = Row::State::failed;
                        row_->response = *error;
                    } else {
                        row_->state = Row::State::complete;
                        if (responses.front()->value != "null") {
                            row_->response = replaceInvalidCharacters(responses.front()->value);
                        }
                    }

                    table.updateContent();

                    if (findRow(Row::State::inProgress)) {
                        return;
                    }

                    if (const auto index = findRow(Row::State::failed)) {
                        setOkButton(true);
                        setCancelButton(true);
                        table.scrollToEnsureRowIsOnscreen(*index);
                        return;
                    }

                    okCallback = [&] {
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

void SendingCommandDialog::resized() {
    Dialog::resized();

    table.setBounds(getContentBounds(true));
}

std::optional<int> SendingCommandDialog::findRow(const Row::State state) const {
    for (const auto [index, row]: juce::enumerate(rows)) {
        if (row.state == state) {
            return (int) index;
        }
    }
    return {};
}

std::string SendingCommandDialog::replaceInvalidCharacters(const std::string &input) {
    std::string output;

    for (char character: input) {
        if ((unsigned char) character > 0x7E) {
            output += "?";
            continue;
        }
        output += character;
    }

    return output;
}

int SendingCommandDialog::getNumRows() {
    return (int) rows.size();
}

void SendingCommandDialog::paintRowBackground(juce::Graphics &g, int rowNumber, int height, int, bool) {
    if (rowNumber >= (int) rows.size()) {
        return; // index may exceed size on Windows if display scaling >100%
    }

    g.setColour(rows[(size_t) rowNumber].connectionPanel.getTag());
    g.fillRect(0, 0, UILayout::tagWidth, height);
}

juce::Component *SendingCommandDialog::refreshComponentForCell(int rowNumber, int columnId, bool, juce::Component *existingComponentToUpdate) {
    if (rowNumber >= (int) rows.size()) {
        return existingComponentToUpdate; // index may exceed size on Windows if display scaling >100%
    }

    delete existingComponentToUpdate;

    switch ((ColumnIds) columnId) {
        case ColumnIds::tag:
            return nullptr;

        case ColumnIds::headingAndResponse:
            class HeadingAndResponse : public juce::Component {
            public:
                HeadingAndResponse(const Row &row)
                    : headingLabel(row.connectionPanel.getHeading()),
                      responseLabel(row.response, UIFonts::getDefaultFont(), juce::Justification::centredRight) {
                    addAndMakeVisible(headingLabel);
                    addAndMakeVisible(responseLabel);
                    switch (row.state) {
                        case Row::State::inProgress:
                            break;
                        case Row::State::failed:
                            responseLabel.setColour(juce::Label::textColourId, UIColours::warning);
                            break;
                        case Row::State::complete:
                            responseLabel.setColour(juce::Label::textColourId, UIColours::success);
                            break;
                    }
                }

                void resized() override {
                    auto bounds = getLocalBounds();
                    if (responseLabel.getText().isNotEmpty()) {
                        responseLabel.setBounds(bounds.removeFromRight((int) std::ceil(responseLabel.getTextWidth())));
                        bounds.removeFromRight(10);
                    }
                    headingLabel.setBounds(bounds);
                }

                SimpleLabel headingLabel;
                SimpleLabel responseLabel;

            private:
                JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeadingAndResponse)
            };

            return new HeadingAndResponse(rows[(size_t) rowNumber]);

        case ColumnIds::icon:
            switch (rows[(size_t) rowNumber].state) {
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

void SendingCommandDialog::timerCallback() {
    stopTimer();

    if (ApplicationSettings::getSingleton().commands.closeSendingCommandDialogWhenComplete) {
        DialogQueue::getSingleton().pop();
    }
}
