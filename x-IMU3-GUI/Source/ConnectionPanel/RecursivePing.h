#pragma once

class RecursivePing {
public:
    RecursivePing(std::shared_ptr<ximu3::Connection> connection_) : connection(connection_) {
        ping();
    }

    void ping() {
        connection->pingAsync([&, weakReference = juce::WeakReference(this)](std::optional<ximu3::XIMU3_PingResponse> response) {
            juce::MessageManager::callAsync([&, weakReference, response] {
                if (weakReference == nullptr) {
                    return;
                }

                if (response.has_value() == false) {
                    ping();
                }
            });
        });
    }

private:
    const std::shared_ptr<ximu3::Connection> connection;

    JUCE_DECLARE_WEAK_REFERENCEABLE(RecursivePing)
};
