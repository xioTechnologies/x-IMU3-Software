import time

import ximu3

network_announcement = ximu3.NetworkAnnouncement()


# Blocking
def print_message(message: ximu3.NetworkAnnouncementMessage) -> None:
    print(
        ", ".join(
            [
                message.device_name,
                message.serial_number,
                str(message.ip_address),
                str(message.tcp_port),
                str(message.udp_send),
                str(message.udp_receive),
                str(message.rssi) + "%",
                str(message.battery) + "%",
                ximu3.charging_status_to_string(message.charging_status),
            ]
        )
    )
    # print(message)  # alternative to above


for message in network_announcement.get_messages_after_short_delay():
    print_message(message)


# Non-blocking
def callback(message: ximu3.NetworkAnnouncementMessage) -> None:
    print_message(message)


network_announcement.add_callback(callback)

time.sleep(60)
