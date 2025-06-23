import time

import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0].device_name} {devices[0].serial_number}")

# Open connection
connection = ximu3.Connection(devices[0].connection_info)

if connection.open() != ximu3.RESULT_OK:
    raise Exception("Unable to open connection")

# Demux
demux = ximu3.Demux(connection, [0x41, 0x42, 0x43])

result = demux.get_result()

print(ximu3.result_to_string(result))

if result == ximu3.RESULT_OK:
    time.sleep(60)

del demux

# Close connection
connection.close()
