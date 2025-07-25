import connection
import helpers
import ximu3

if helpers.yes_or_no("Search for connections?"):
    devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_SERIAL)

    if not devices:
        raise Exception("No serial connections available")

    print(f"Found {devices[0].device_name} {devices[0].serial_number}")

    connection_info = devices[0].connection_info

    connection.run(connection_info)
else:
    connection_info = ximu3.SerialConnectionInfo("COM1", 115200, False)  # replace with actual connection info

    connection.run(connection_info)
