#include "ApplicationSettings.h"
#include "ProcessCommandDialog.h"

ProcessCommandDialog::ProcessCommandDialog(const juce::String &title, const std::string &prefix_, const int timeout_, const bool saveOnComplete_, const std::vector<ConnectionPanel *> &connectionPanels_)
    : CommandProgressDialog(title, connectionPanels_), prefix(prefix_), timeout(timeout_), saveOnComplete(saveOnComplete_) {
    onStart(false);
}

ProcessCommandDialog::~ProcessCommandDialog() {
    *stopPolling = true;
}

void ProcessCommandDialog::onStart(const bool retry) {
    *stopPolling = true;
    stopPolling = std::make_shared<std::atomic<bool> >(false);

    for (int index = 0; index < (int) connectionPanels.size(); index++) {
        setInProgress(index, {});

        juce::Timer::callAfterDelay(retry ? retryDelay : 0, [&, index, self = SafePointer(this)] {
            if (self == nullptr) {
                return;
            }

            connectionPanels[(size_t) index]->sendCommands({"{\"" + prefix + "_start\":" + std::to_string(timeout) + "}"}, this, [&, index](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
                const auto &response = responses.front();

                if (response.has_value() == false) {
                    setFailed(index, "No response");
                    return;
                }

                if (response->error) {
                    setFailed(index, *response->error);
                    return;
                }

                setInProgress(index, 0);
                startPolling(index);
            });
        });
    }
}

void ProcessCommandDialog::onComplete() {
    for (auto &connectionPanel: connectionPanels) {
        connectionPanel->sendCommands({"{\"" + prefix + "_complete\":null}"});
    };
}

bool ProcessCommandDialog::completeAllowed() {
    return ApplicationSettings::getSingleton().commands.allowEarlyCompletion;
}

void ProcessCommandDialog::onCancel() {
    for (auto &connectionPanel: connectionPanels) {
        connectionPanel->sendCommands({"{\"" + prefix + "_abort\":null}"});
    };
}

void ProcessCommandDialog::startPolling(const int index) {
    juce::Thread::launch([stopPolling_ = stopPolling, index, prefix_ = prefix, connection = connectionPanels[(size_t) index]->getConnection(), this] {
            const auto callAsync = [=](auto callback) {
                juce::MessageManager::callAsync([=] {
                    if (stopPolling_->load() == false) {
                        callback();
                    }
                });
            };

            while (stopPolling_->load() == false) {
                juce::Thread::sleep(200);

                const auto response = connection->sendCommands({"{\"" + prefix_ + "_progress\":null}"},
                                                               ApplicationSettings::getSingleton().commands.retries,
                                                               ApplicationSettings::getSingleton().commands.timeout).front();

                if (response.has_value() == false) {
                    callAsync([=, this] {
                        setFailed(index, "No response");
                    });
                    continue;
                }

                if (response->error) {
                    callAsync([=, this] {
                        setFailed(index, *response->error);
                    });
                    return;
                }

                if (response->valueType == ximu3::XIMU3_JsonTypeString) {
                    callAsync([=, this] {
                        if (saveOnComplete) {
                            save(index);
                            return;
                        }

                        setComplete(index);
                    });
                    return;
                }

                callAsync([=, this] {
                    setInProgress(index, juce::String(response->value).getIntValue());
                });
            }
        }
    );
}

void ProcessCommandDialog::save(const int index) {
    connectionPanels[(size_t) index]->sendCommands({"{\"save\":null}"}, this, [&, index](const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
        const auto &response = responses.front();

        if (response.has_value() == false) {
            setFailed(index, "No response");
            return;
        }

        if (response->error) {
            setFailed(index, *response->error);
            return;
        }

        setComplete(index);
    });
}
