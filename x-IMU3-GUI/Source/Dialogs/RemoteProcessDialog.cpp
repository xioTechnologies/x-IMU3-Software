#include "ApplicationSettings.h"
#include "RemoteProcessDialog.h"

RemoteProcessDialog::RemoteProcessDialog(const juce::String &icon_,
                                         const juce::String &dialogTitle,
                                         const std::vector<ConnectionPanel *> &connectionPanels_,
                                         juce::ThreadPool &threadPool_,
                                         const std::string &prefix_,
                                         const std::optional<int> timeout_,
                                         const bool showCompleteValue_,
                                         const bool saveOnComplete_)
    : CommandProgressDialog(icon_, dialogTitle, connectionPanels_, ApplicationSettings::getSingleton().commands.allowEarlyCompletion),
      prefix(prefix_),
      timeout(timeout_),
      showCompleteValue(showCompleteValue_),
      saveOnComplete(saveOnComplete_),
      threadPool(threadPool_) {
    onStart(false);
}

RemoteProcessDialog::~RemoteProcessDialog() {
    *stopPolling = true;
}

void RemoteProcessDialog::onStart(const bool retry) {
    *stopPolling = true;
    stopPolling = std::make_shared<std::atomic<bool> >(false);

    for (int index = 0; index < (int) connectionPanels.size(); index++) {
        setInProgress(index, {});

        juce::Timer::callAfterDelay(retry ? retryDelay : 0, [&, index, self = SafePointer(this)] {
            if (self == nullptr) {
                return;
            }

            const std::string command = "{\"" + prefix + "_start\":" + (timeout ? std::to_string(*timeout) : "null") + "}";

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

                setInProgress(index, 0);
                startPolling(index);
            });
        });
    }
}

void RemoteProcessDialog::onComplete() {
    for (auto &connectionPanel: connectionPanels) {
        connectionPanel->sendCommands({"{\"" + prefix + "_complete\":null}"});
    };
}

void RemoteProcessDialog::onCancel() {
    for (auto &connectionPanel: connectionPanels) {
        connectionPanel->sendCommands({"{\"" + prefix + "_abort\":null}"});
    };
}

void RemoteProcessDialog::startPolling(const int index) {
    threadPool.addJob([stopPolling_ = stopPolling, index, prefix_ = prefix, connection = connectionPanels[(size_t) index]->getConnection(), this] {
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
                    break;
                }

                if (response->valueType == ximu3::XIMU3_JsonTypeNumber) {
                    callAsync([=, this] {
                        setInProgress(index, juce::String(response->value).getIntValue());
                    });
                    continue;
                }

                callAsync([=, this] {
                    if (saveOnComplete) {
                        save(index);
                        return;
                    }

                    if (showCompleteValue && (response->valueType != ximu3::XIMU3_JsonTypeNull)) {
                        setComplete(index, replaceInvalidCharacters(response->value));
                    } else {
                        setComplete(index);
                    }
                });
                break;
            }
        }
    );
}

void RemoteProcessDialog::save(const int index) {
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
