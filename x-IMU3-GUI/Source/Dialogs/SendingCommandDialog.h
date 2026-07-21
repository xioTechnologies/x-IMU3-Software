#pragma once

#include "CommandProgressDialog.h"

class SendingCommandDialog : public CommandProgressDialog {
public:
    SendingCommandDialog(const std::vector<ConnectionPanel *> &connectionPanels_, const std::string &command_);

protected:
    void onStart(const bool retry) override;

    void onComplete() override;

    void onCancel() override;

private:
    const std::string command;

    static std::string replaceInvalidCharacters(const std::string &input);
};
