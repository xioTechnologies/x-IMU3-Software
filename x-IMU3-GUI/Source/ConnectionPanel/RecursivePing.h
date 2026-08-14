#pragma once

class RecursivePing {
public:
    RecursivePing(std::shared_ptr<ximu3::Connection> connection_) : connection(connection_) {
        ping();
    }

    void ping() {
        const juce::WeakReference weakReference(this);

        connection->pingAsync([&, connection_ = connection, weakReference](std::optional<ximu3::XIMU3_PingResponse> response) {
            juce::MessageManager::callAsync([&, connection_, response, weakReference] {
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
