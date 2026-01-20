import connection
import helpers
import ximu3

if helpers.yes_or_no("Search for connections?"):
    messages = ximu3.NetworkAnnouncement().get_messages_after_short_delay()

    if not messages:
        raise Exception("No UDP connections available")

    print(f"Found {messages[0].device_name} {messages[0].serial_number}")

    connection_info = messages[0].to_udp_connection_info()

    connection.run(connection_info)
else:
    connection_info = ximu3.UdpConnectionInfo("192.168.1.1", 9000, 8000)  # replace with actual connection info

    connection.run(connection_info)
