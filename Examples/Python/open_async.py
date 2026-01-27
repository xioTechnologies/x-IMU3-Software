import time

import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

time.sleep(1)  # wait for OS to release port

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0]}")

# Open connection
connection = ximu3.Connection(devices[0].connection_config)


def callback(result: int) -> None:
    if result != ximu3.RESULT_OK:
        raise Exception(f"Unable to open {connection.get_config()}. {ximu3.result_to_string(result)}.")

    print(ximu3.result_to_string(result))


connection.open_async(callback)

# Close connection
time.sleep(3)

connection.close()
