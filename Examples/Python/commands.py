import helpers
import time
import ximu3

# Find USB device
print("Searching for connections")

devices = ximu3.PortScanner.scan_filter(ximu3.CONNECTION_TYPE_USB)

if not devices:
    raise Exception("No USB connections available")

print("Found " + devices[0].device_name + " - " + devices[0].serial_number)

# Open connection
connection = ximu3.Connection(devices[0].connection_info)

if connection.open() != ximu3.RESULT_OK:
    raise Exception("Unable to open connection")

print("Connection successful")

# Define read/write setting commands
commands = ["{\"deviceName\":null}",  # change null to a value to write setting
            "{\"serialNumber\":null}",
            "{\"firmwareVersion\":null}",
            "{\"bootloaderVersion\":null}",
            "{\"hardwareVersion\":null}",
            "{\"invalidSettingKey\":null}"]  # this command is deliberately invalid to demonstrate a failed command


def print_responses(responses):
    print(str(len(responses)) + " commands confirmed")

    for response in responses:
        print(response)


def callback(responses):
    print_responses(responses)


# Send commands
if helpers.yes_or_no("Use async implementation?"):
    connection.send_commands_async(commands, 2, 500, callback)
    time.sleep(3)
else:
    print_responses(connection.send_commands(commands, 2, 500))

# Close connection
connection.close()
