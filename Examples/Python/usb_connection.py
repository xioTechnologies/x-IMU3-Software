import connection
import helpers
import ximu3

if helpers.yes_or_no("Search for connections?"):
    print("Searching for connections")

    devices = ximu3.SerialDiscovery.scan_filter(2000, "USB")

    if len(devices) == 0:
        raise Exception("No USB connections available")

    print("Found " + devices[0].device_name + " - " + devices[0].serial_number)

    connection.run(devices[0].connection_info)
else:
    connection.run(ximu3.UsbConnectionInfo("COM1"))
