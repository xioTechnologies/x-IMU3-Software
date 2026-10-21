#include "../ApplicationSettings.h"
#include "SendingCommandDialog.h"

SendingCommandDialog::SendingCommandDialog(const std::vector<ConnectionPanel *> &connectionPanels_, const std::string &command_)
    : CommandProgressDialog(BinaryData::json_svg, "Sending " + replaceInvalidCharacters(command_), connectionPanels_),
      command(command_) {
    onStart(false);
}

void SendingCommandDialog::onStart(const bool retry) {
    for (int index = 0; index < (int) connectionPanels.size(); index++) {
        if (getStatus(index) != CommandProgressTable::Status::complete) {
            setInProgress(index, {});
        }
    }

    juce::Timer::callAfterDelay(retry ? retryDelay : 0, [self = SafePointer(this)] {
        if (self != nullptr) {
            self->send();
        }
    });
}

void SendingCommandDialog::onComplete() {
}

void SendingCommandDialog::onCancel() {
}

void SendingCommandDialog::send() {
    for (int index = 0; index < (int) connectionPanels.size(); index++) {
        if (getStatus(index) != CommandProgressTable::Status::inProgress) {
            continue;
        }

        connectionPanels[(size_t) index]->sendCommand(command, this, [this, index](const std::optional<ximu3::CommandMessage> &response) {
            handleResponse(index, response);

            if (ApplicationSettings::getSingleton().commands.sendConsecutively) {
                send();
            }
        });

        if (ApplicationSettings::getSingleton().commands.sendConsecutively) {
            break;
        }
    }
}

void SendingCommandDialog::handleResponse(const int index, const std::optional<ximu3::CommandMessage> &response) {
    if (response.has_value() == false) {
        setFailed(index, "No response");
        return;
    }

    if (response->error) {
        setFailed(index, *response->error);
        return;
    }

    if (response->valueType == ximu3::XIMU3_JsonTypeNull) {
        setComplete(index);
    } else {
        setComplete(index, replaceInvalidCharacters(response->value));
    }
}
