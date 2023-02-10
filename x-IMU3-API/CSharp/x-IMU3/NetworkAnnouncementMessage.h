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

		property String^ IPAddress
		{
			String^ get()
			{
				return gcnew String(message->ip_address);
			}
		}

		property uint16_t TcpPort
		{
			uint16_t get()
			{
				return message->tcp_port;
			}
		}

		property uint16_t UdpSend
		{
			uint16_t get()
			{
				return message->udp_send;
			}
		}

		property uint16_t UdpReceive
		{
			uint16_t get()
			{
				return message->udp_receive;
			}
		}

		property int32_t Rssi
		{
			int32_t get()
			{
				return message->rssi;
			}
		}

		property int32_t Battery
		{
			int32_t get()
			{
				return message->battery;
			}
		}

		property ChargingStatus ChargingStatus
		{
			Ximu3::ChargingStatus get()
			{
				return (Ximu3::ChargingStatus)message->charging_status;
			}
		}

		TcpConnectionInfo^ ToTcpConnectionInfo()
		{
			return gcnew TcpConnectionInfo(&ximu3::XIMU3_network_announcement_message_to_tcp_connection_info(*message));
		}

		UdpConnectionInfo^ ToUdpConnectionInfo()
		{
			return gcnew UdpConnectionInfo(&ximu3::XIMU3_network_announcement_message_to_udp_connection_info(*message));
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
