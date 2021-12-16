import helpers
import ximu3

# Create connection info
connection_info = ximu3.UsbConnectionInfo("COM1")

# Open and ping
connection = ximu3.Connection(connection_info)


def print_ping_response(ping_response):
    print(ping_response.interface + ", " +
          ping_response.device_name + " - " +
          ping_response.serial_number)
    # print(ping_response.to_string()) # alternative to above


def callback(result):
    if result != "Ok":
        print("Unable to open connection")
        return

    print_ping_response(connection.ping())


if helpers.yes_or_no("Use async implementation?"):
    connection.open_async(callback)
    helpers.wait(3)
else:
    connection.open()
    print_ping_response(connection.ping())

# Close connection
connection.close()
