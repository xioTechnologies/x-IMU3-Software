import time

import helpers
import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0]}")

# Open connection
connection = ximu3.Connection(devices[0].connection_config)

connection.open()

# Ping


def print_response(response: ximu3.PingResponse | None) -> None:
    if response is None:
        raise Exception("No response")

    print(", ".join([response.interface, response.device_name, response.serial_number]))
    # print(response)  # alternative to above


def callback(response: ximu3.PingResponse) -> None:
    print_response(response)


if helpers.yes_or_no("Use async implementation?"):
    connection.ping_async(callback)
    time.sleep(3)
else:
    print_response(connection.ping())

# Close connection
connection.close()
