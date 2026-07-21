#include "SendingCommandDialog.h"

SendingCommandDialog::SendingCommandDialog(const std::vector<ConnectionPanel *> &connectionPanels_, const std::string &command_)
    : CommandProgressDialog("Sending " + replaceInvalidCharacters(command_), connectionPanels_),
      command(command_) {
    onStart(false);
}

void SendingCommandDialog::onStart(const bool retry) {
    for (int index = 0; index < (int) connectionPanels.size(); index++) {
        if (getStatus(index) == CommandProgressTable::Status::complete) {
            continue;
        }

        setInProgress(index, {});

        juce::Timer::callAfterDelay(retry ? retryDelay : 0, [&, index, self = SafePointer(this)] {
            if (self == nullptr) {
                return;
            }

            connectionPanels[(size_t) index]->sendCommands({command}, this, [&, index](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
                const auto &response = responses.front();

                if (response.has_value() == false) {
                    setFailed(index, "No response");
                    return;
                }

                if (response->error) {
                    setFailed(index, *response->error);
                    return;
                }

                if (response->value == "null") {
                    setComplete(index);
                    return;
                }

                setComplete(index, replaceInvalidCharacters(responses.front()->value));
            });
        });
    }
}

void SendingCommandDialog::onComplete() {
}

void SendingCommandDialog::onCancel() {
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
