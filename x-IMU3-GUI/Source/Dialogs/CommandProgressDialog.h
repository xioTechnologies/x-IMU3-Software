#pragma once

#include "../ConnectionPanel/ConnectionPanel.h"
#include "../Widgets/CommandProgressTable.h"
#include "../Widgets/CustomToggleButton.h"
#include "Dialog.h"

class CommandProgressDialog : public Dialog {
public:
    static constexpr int retryDelay = 250; // to improve UX

    CommandProgressDialog(const juce::String &dialogTitle, const std::vector<ConnectionPanel *> &connectionPanels_);

    void resized() override;

    void setInProgress(int rowIndex, std::optional<int> percentage);

    void setComplete(int rowIndex, const juce::String &value = "");

    void setFailed(int rowIndex, const juce::String &error);

    CommandProgressTable::Status getStatus() const;

    CommandProgressTable::Status getStatus(const int rowIndex) const;

protected:
    const std::vector<ConnectionPanel *> connectionPanels;

    virtual void onStart(const bool retry) = 0;

    virtual void onComplete() = 0;

    virtual bool completeAllowed() = 0;

    virtual void onCancel() = 0;

private:
    CustomToggleButton closeWhenCompleteButton{"Close When Complete"};
    CommandProgressTable table;

    void statusChanged();
};
