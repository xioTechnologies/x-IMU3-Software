#pragma once

#include "../../C/Ximu3.h"
#include "ChargingStatus.h"
#include "ConnectionInfo.h"

using namespace System;

namespace Ximu3
{
    public ref class NetworkAnnouncementMessage
    {
    internal:
        NetworkAnnouncementMessage(ximu3::XIMU3_NetworkAnnouncementMessage message) : message{ new ximu3::XIMU3_NetworkAnnouncementMessage{message} }
        {
        }

    public:
        ~NetworkAnnouncementMessage()
        {
            delete message;
        }

        property String^ DeviceName
        {
            String^ get()
            {
                return gcnew String(message->device_name);
            }
        }

        property String^ SerialNumber
        {
            String^ get()
            {
                return gcnew String(message->serial_number);
            }
        }

        property uint32_t Rssi
        {
            uint32_t get()
            {
                return message->rssi;
            }
        }

        property uint32_t Battery
        {
            uint32_t get()
            {
                return message->battery;
            }
        }

        property ChargingStatus Status
        {
            ChargingStatus get()
            {
                return (ChargingStatus)message->status;
            }
        }

        property TcpConnectionInfo^ TcpConnectionInfo
        {
            Ximu3::TcpConnectionInfo^ get()
            {
                return gcnew Ximu3::TcpConnectionInfo(&message->tcp_connection_info);
            }
        }

        property UdpConnectionInfo^ UdpConnectionInfo
        {
            Ximu3::UdpConnectionInfo^ get()
            {
                return gcnew Ximu3::UdpConnectionInfo(&message->udp_connection_info);
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_network_announcement_message_to_string(*message));
        }

    internal:
        static array<NetworkAnnouncementMessage^>^ ToArrayAndFree(const ximu3::XIMU3_NetworkAnnouncementMessages messages) {
            array<NetworkAnnouncementMessage^>^ messagesArray = gcnew array<NetworkAnnouncementMessage^>(messages.length);

            for (uint32_t index = 0; index < messages.length; index++)
            {
                messagesArray[index] = gcnew NetworkAnnouncementMessage(messages.array[index]);
            }

            ximu3::XIMU3_network_announcement_messages_free(messages);
            return messagesArray;
        }

    private:
        ximu3::XIMU3_NetworkAnnouncementMessage* message;
    };
}
