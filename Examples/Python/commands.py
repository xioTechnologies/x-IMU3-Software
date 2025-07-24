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

result = connection.open()

if result != ximu3.RESULT_OK:
    raise Exception(f"Unable to open connection. {ximu3.result_to_string(result)}.")

# Example commands
commands = [
    '{"device_name":"Foobar"}',  # write "Foobar" to device name
    '{"serial_number":null}',  # read serial number
    '{"firmware_version":null}',  # read firmware version
    '{"invalid_key":null}',  # invalid key to demonstrate an error response
]

# Send commands


def print_responses(responses):
    print(f"{str(len(responses))} responses")

    for response in responses:
        response = ximu3.CommandMessage.parse(response)

        if response.error:
            print(response.error)
            continue

        print(f"{response.key} : {response.value}")


def callback(responses):
    print_responses(responses)


if helpers.yes_or_no("Use async implementation?"):
    connection.send_commands_async(commands, 2, 500, callback)
    time.sleep(3)
else:
    print_responses(connection.send_commands(commands, 2, 500))

# Close connection
connection.close()
