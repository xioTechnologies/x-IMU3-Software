import connection
import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0].device_name} {devices[0].serial_number}")

# Open connection
usb_connection = ximu3.Connection(devices[0].connection_info)

if usb_connection.open() != ximu3.RESULT_OK:
    raise Exception("Unable to open connection")

connection_info = ximu3.MuxConnectionInfo(0x41, usb_connection)

connection.run(connection_info)
