#pragma once

#include "CommandProgressDialog.h"

class ProcessCommandDialog : public CommandProgressDialog {
public:
    ProcessCommandDialog(const juce::String &title, const std::string &prefix_, const int timeout_, const bool saveOnComplete_, const std::vector<ConnectionPanel *> &connectionPanels_);

    ~ProcessCommandDialog() override;

protected:
    void onStart(const bool retry) override;

    void onComplete() override;

    bool completeAllowed() override;

    void onCancel() override;

private:
    const std::string prefix; // "<prefix>_start", "<prefix>_progress", "<prefix>_complete", "<prefix>_complete"
    const int timeout;
    const bool saveOnComplete;
    std::shared_ptr<std::atomic<bool> > stopPolling = std::make_shared<std::atomic<bool> >(false);

    void startPolling(const int index);

    void save(const int index);
};
