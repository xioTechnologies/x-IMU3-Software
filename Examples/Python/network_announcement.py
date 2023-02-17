import helpers
import time
import ximu3


def print_message(message):
    print(message.device_name + ", " +
          message.serial_number + ", " +
          str(message.ip_address) + ", " +
          str(message.tcp_port) + ", " +
          str(message.udp_send) + ", " +
          str(message.udp_receive) + ", " +
          str(message.rssi) + "%, " +
          str(message.battery) + "%, " +
          ximu3.charging_status_to_string(message.charging_status))
    # print(message.to_string())  # alternative to above


def callback(message):
    print_message(message)


network_announcement = ximu3.NetworkAnnouncement()

if helpers.yes_or_no("Use async implementation?"):
    network_announcement.add_callback(callback)
    time.sleep(60)
else:
    for message in network_announcement.get_messages():
        print_message(message)
