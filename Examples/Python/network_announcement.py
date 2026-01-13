import time

import helpers
import ximu3


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


def callback(message: ximu3.NetworkAnnouncementMessage) -> None:
    print_message(message)


network_announcement = ximu3.NetworkAnnouncement()

if helpers.yes_or_no("Use async implementation?"):
    network_announcement.add_callback(callback)

    time.sleep(60)
else:
    for message in network_announcement.get_messages():
        print_message(message)
