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
        const juce::WeakReference weakReference (this);

        connection->pingAsync([&, connection_ = connection, weakReference](ximu3::XIMU3_PingResponse response)
        {
            juce::MessageManager::callAsync([&, connection_, response, weakReference]
            {
                if (weakReference == nullptr)
                {
                    return;
                }

                if (response.result == ximu3::XIMU3_ResultOk)
                {
                    callback(response.device_name, response.serial_number);
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
