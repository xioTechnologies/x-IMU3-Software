import time

import helpers
import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0]}")

# Open connection
connection = ximu3.Connection(devices[0].connection_info)

result = connection.open()

if result != ximu3.RESULT_OK:
    raise Exception(f"Unable to open {connection.get_info()}. {ximu3.result_to_string(result)}.")

# Ping


def print_ping_response(ping_response: ximu3.PingResponse | None) -> None:
    if ping_response is None:
        raise Exception("No response")

    print(", ".join([ping_response.interface, ping_response.device_name, ping_response.serial_number]))
    # print(ping_response)  # alternative to above


def callback(ping_response: ximu3.PingResponse) -> None:
    print_ping_response(ping_response)


if helpers.yes_or_no("Use async implementation?"):
    connection.ping_async(callback)
    time.sleep(3)
else:
    print_ping_response(connection.ping())

# Close connection
connection.close()
