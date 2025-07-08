import time

import helpers
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

# Ping


def print_ping_response(response):
    if response.result == ximu3.RESULT_OK:
        print(", ".join([response.interface, response.device_name, response.serial_number]))
        # print(response.to_string())  # alternative to above
    else:
        print("No response")


def callback(response):
    print_ping_response(response)


if helpers.yes_or_no("Use async implementation?"):
    connection.ping_async(callback)
    time.sleep(3)
else:
    print_ping_response(connection.ping())

# Close connection
connection.close()
