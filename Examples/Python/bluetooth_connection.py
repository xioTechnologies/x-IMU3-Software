import time

import connection
import helpers
import ximu3

if helpers.yes_or_no("Search for connections?"):
    devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_BLUETOOTH)

    time.sleep(1)  # wait for OS to release port

    if not devices:
        raise Exception("No Bluetooth connections available")

    print(f"Found {devices[0]}")

    config = devices[0].config

    connection.run(config)
else:
    config = ximu3.BluetoothConnectionConfig("COM1")  # replace with actual connection config

    connection.run(config)
