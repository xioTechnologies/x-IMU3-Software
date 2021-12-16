import connection
import helpers
import ximu3

if helpers.yes_or_no("Search for connections?"):
    print("Searching for connections")

    devices = ximu3.NetworkDiscovery.scan(2000)

    if len(devices) == 0:
        raise Exception("No UDP connections available")

    print("Found " + devices[0].device_name + " - " + devices[0].serial_number)

    connection.run(devices[0].udp_connection_info)
else:
    connection.run(ximu3.UdpConnectionInfo("192.168.1.1", 9000, 8000))
