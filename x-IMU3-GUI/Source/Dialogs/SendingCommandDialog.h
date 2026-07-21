#pragma once

#include "CommandProgressDialog.h"

class SendingCommandDialog : public CommandProgressDialog {
public:
    SendingCommandDialog(const std::string &command_, const std::vector<ConnectionPanel *> &connectionPanels_);

protected:
    void onStart(const bool retry) override;

    void onComplete() override;

    bool completeAllowed() override;

    void onCancel() override;

private:
    const std::string command;

    static std::string replaceInvalidCharacters(const std::string &input);
};
