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

# Example commands
commands = [
    '{"device_name":"Foobar"}',  # write "Foobar" to device name
    '{"serial_number":null}',  # read serial number
    '{"firmware_version":null}',  # read firmware version
    '{"invalid_key":null}',  # invalid key to demonstrate an error response
]


def print_responses(responses):
    print(f"{str(len(responses))} responses received")
    
    for response in responses:
        response = ximu3.CommandMessage.parse(response)

        if response.error:
            print(response.error)
            return

        print(f"{response.key} : {response.value}")


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
