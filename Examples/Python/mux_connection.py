import time

import connection
import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

time.sleep(1)  # wait for OS to release port

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0]}")

# Open connection
usb_connection = ximu3.Connection(devices[0].connection_config)

usb_connection.open()

config = ximu3.MuxConnectionConfig(0x41, usb_connection)

connection.run(config)
