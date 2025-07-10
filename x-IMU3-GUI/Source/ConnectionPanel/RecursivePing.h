#pragma once

class RecursivePing
{
public:
    using Callback = std::function<void(const juce::String& deviceName, const juce::String& serialNumber)>;

    RecursivePing(std::shared_ptr<ximu3::Connection> connection_, Callback callback_) : connection(connection_), callback(callback_)
    {
        ping();
    }

    void ping()
    {
        const juce::WeakReference weakReference(this);

        connection->pingAsync([&, connection_ = connection, weakReference](ximu3::XIMU3_PingResponse pingResponse)
        {
            juce::MessageManager::callAsync([&, connection_, pingResponse, weakReference]
            {
                if (weakReference == nullptr)
                {
                    return;
                }

                if (pingResponse.result == ximu3::XIMU3_ResultOk)
                {
                    callback(pingResponse.device_name, pingResponse.serial_number);
                    return;
                }

                ping();
            });
        });
    }

private:
    std::shared_ptr<ximu3::Connection> connection;
    const Callback callback;

    JUCE_DECLARE_WEAK_REFERENCEABLE(RecursivePing)
};
