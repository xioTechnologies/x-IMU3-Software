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


# Send accessory data
def print_data(direction: str, data: bytes) -> None:
    print(f"{direction} {' '.join(f'{d:02X}' for d in data)}")


data = b"Any value \x00 to \xff\n"

data_as_json = ximu3.CommandMessage.bytes_to_json(data)

response = connection.send_command(f'{{"accessory":{data_as_json}}}')

if response is None:
    print("No response")
elif response.error is not None:
    print(response.error)
else:
    print_data("TX", data)

# Receive accessory data
time.sleep(1)

message = connection.get_serial_accessory_message()

if message:
    print_data("RX", message.bytes)

# Close connection
connection.close()
