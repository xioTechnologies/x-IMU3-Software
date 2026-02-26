import time

import connection
import helpers
import ximu3

# Search for connection
usb_devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

time.sleep(1)  # wait for OS to release port

if not usb_devices:
    raise Exception("No USB connections available")

print(f"Found {usb_devices[0]}")

# Open connection
usb_connection = ximu3.Connection(usb_devices[0].connection_config).open()

# Mux connection
if helpers.yes_or_no("Search for connections?"):
    mux_devices = ximu3.MuxScanner.scan(usb_connection)

    if not mux_devices:
        raise Exception("No mux connections available")

    print(f"Found {mux_devices[0]}")

    config = mux_devices[0].connection_config

    connection.run(config)
else:
    config = ximu3.MuxConnectionConfig(0x41, usb_connection)  # replace with actual connection config

    connection.run(config)

# Close connection
usb_connection.close()
