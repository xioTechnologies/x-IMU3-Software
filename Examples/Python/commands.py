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
    raise Exception(f"Unable to open {connection.get_info().to_string()}. {ximu3.result_to_string(result)}.")

# Example commands
commands = [
    '{"device_name":"Foobar"}',  # write "Foobar" to device name
    '{"serial_number":null}',  # read serial number
    '{"firmware_version":null}',  # read firmware version
    '{"invalid_key":null}',  # invalid key to demonstrate an error response
]

# Send commands


def print_responses(responses: list[ximu3.CommandMessage | None]) -> None:
    for response in responses:
        if response is None:
            print("No response")
            continue

        if response.error:
            print(response.error)
            continue

        print(f"{response.key} : {response.value}")


def callback(responses: list[ximu3.CommandMessage | None]) -> None:
    print_responses(responses)


if helpers.yes_or_no("Use async implementation?"):
    connection.send_commands_async(commands, callback)

    time.sleep(3)
else:
    print_responses(connection.send_commands(commands))

# Close connection
connection.close()
