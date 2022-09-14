import helpers
import ximu3


def print_message(message):
    print(message.device_name + " - " +
          message.serial_number + ", RSSI: " +
          str(message.rssi) + "%, Battery: " +
          str(message.battery) + "%, " +
          message.status + ", " +
          message.tcp_connection_info.to_string() + ", " +
          message.udp_connection_info.to_string())
    # print(message.to_string())  # alternative to above


def callback(message):
    print_message(message)


network_announcement = ximu3.NetworkAnnouncement()

if helpers.yes_or_no("Use async implementation?"):
    network_announcement.add_callback(callback)
    helpers.wait(-1)
else:
    for message in network_announcement.get_messages():
        print_message(message)
