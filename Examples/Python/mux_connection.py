import connection
import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0]}")

# Open connection
usb_connection = ximu3.Connection(devices[0].connection_info)

result = usb_connection.open()

if result != ximu3.RESULT_OK:
    raise Exception(f"Unable to open {usb_connection.get_info().to_string()}. {ximu3.result_to_string(result)}.")

connection_info = ximu3.MuxConnectionInfo(0x41, usb_connection)

connection.run(connection_info)
