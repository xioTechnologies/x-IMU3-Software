#include "../ApplicationSettings.h"
#include "CommandProgressDialog.h"

CommandProgressDialog::CommandProgressDialog(const juce::String &dialogTitle, const std::vector<ConnectionPanel *> &connectionPanels_)
    : Dialog(BinaryData::progress_svg, dialogTitle, "OK", "Cancel", &closeWhenCompleteButton, 175, true),
      connectionPanels(connectionPanels_) {
    addAndMakeVisible(table);
    for (auto &connectionPanel: connectionPanels) {
        table.addRow(connectionPanel->getColourTag(), connectionPanel->getHeading());
    }

    addAndMakeVisible(closeWhenCompleteButton);
    closeWhenCompleteButton.setClickingTogglesState(true);
    closeWhenCompleteButton.setToggleState(ApplicationSettings::getSingleton().commands.closeSendingCommandDialogWhenComplete, juce::dontSendNotification);
    closeWhenCompleteButton.onClick = [&] {
        ApplicationSettings::getSingleton().commands.closeSendingCommandDialogWhenComplete = closeWhenCompleteButton.getToggleState();
    };

    cancelCallback = [&] {
        onCancel();
        return true;
    };

    setSize(600, calculateHeight(0) + margin + table.getIdealHeight());
}

void CommandProgressDialog::resized() {
    Dialog::resized();
    table.setBounds(getContentBounds(true));
}

void CommandProgressDialog::setInProgress(int rowIndex, std::optional<int> percentage) {
    table.setInProgress(rowIndex, percentage);
    statusChanged();
}

void CommandProgressDialog::setComplete(int rowIndex, const juce::String &value) {
    table.setComplete(rowIndex, value);
    statusChanged();
}

void CommandProgressDialog::setFailed(int rowIndex, const juce::String &error) {
    table.setFailed(rowIndex, error);
    statusChanged();
}

CommandProgressTable::Status CommandProgressDialog::getStatus() const {
    return table.getStatus();
}

CommandProgressTable::Status CommandProgressDialog::getStatus(const int rowIndex) const {
    return table.getStatus(rowIndex);
}

void CommandProgressDialog::statusChanged() {
    switch (table.getStatus()) {
        case CommandProgressTable::Status::inProgress:
            setCancelButton(true, "Cancel");

            if (completeAllowed()) {
                setOkButton(true, "Complete");
                okCallback = [&] {
                    onComplete();
                    return false;
                };
            } else {
                setOkButton(false, "Retry");
            }
            break;

        case CommandProgressTable::Status::complete:
            setCancelButton(false, "Cancel");
            setOkButton(true, "Close");

            okCallback = nullptr;

            juce::Timer::callAfterDelay(1000, [&, ptr = SafePointer(this)] {
                if (ptr && ApplicationSettings::getSingleton().commands.closeSendingCommandDialogWhenComplete) {
                    DialogQueue::getSingleton().pop();
                }
            });
            break;

        case CommandProgressTable::Status::failed:
            setCancelButton(true, "Cancel");
            setOkButton(true, "Retry");

            okCallback = [&] {
                onStart(true);
                return false;
            };
            break;
    }
}
