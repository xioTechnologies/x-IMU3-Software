import helpers
import time
import ximu3

# Find USB device
print("Searching for connections")

devices = ximu3.PortScanner.scan_filter(ximu3.CONNECTION_TYPE_USB)

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0].device_name} {devices[0].serial_number}")

# Open connection
connection = ximu3.Connection(devices[0].connection_info)

if connection.open() != ximu3.RESULT_OK:
    raise Exception("Unable to open connection")

print("Connection successful")

# Define read/write setting commands
commands = [
    '{"device_name":null}',  # change null to a value to write setting
    '{"serial_number":null}',
    '{"firmware_version":null}',
    '{"bootloader_version":null}',
    '{"hardware_version":null}',
    '{"invalid_setting_key":null}',  # this command is deliberately invalid to demonstrate a failed command
]


def print_responses(responses):
    print(f"{str(len(responses))} commands confirmed")

    for response in responses:
        print(response)


def callback(responses):
    print_responses(responses)


# Send commands
if helpers.ask_question("Use async implementation?"):
    connection.send_commands_async(commands, 2, 500, callback)
    time.sleep(3)
else:
    print_responses(connection.send_commands(commands, 2, 500))

# Close connection
connection.close()
