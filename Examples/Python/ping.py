import time

import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

time.sleep(1)  # wait for OS to release port

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0]}")

# Open connection
connection = ximu3.Connection(devices[0].connection_config).open()


# Ping (blocking)
def print_response(response: ximu3.PingResponse | None) -> None:
    if not response:
        raise Exception("No response")

    print(", ".join([response.interface, response.device_name, response.serial_number]))
    # print(response)  # alternative to above


response = connection.ping()

print_response(response)


# Ping (non-blocking)
def callback(response: ximu3.PingResponse) -> None:
    print_response(response)


connection.ping_async(callback)

time.sleep(3)

# Close connection
connection.close()
