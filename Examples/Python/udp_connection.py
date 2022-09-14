import connection
import helpers
import ximu3

if helpers.yes_or_no("Search for connections?"):
    print("Searching for connections")

    messages = ximu3.NetworkAnnouncement().get_messages_after_short_delay()

    if not messages:
        raise Exception("No UDP connections available")

    print("Found " + messages[0].device_name + " - " + messages[0].serial_number)

    connection.run(messages[0].udp_connection_info)
else:
    connection.run(ximu3.UdpConnectionInfo("192.168.1.1", 9000, 8000))
