import helpers
import time
import ximu3

# Create connection info
connection_info = ximu3.UsbConnectionInfo("COM1")

# Open and ping
connection = ximu3.Connection(connection_info)


def print_ping_response(ping_response):
    if ping_response.result != ximu3.RESULT_OK:
        print("Ping failed")
        return

    print(ping_response.interface + ", " +
          ping_response.device_name + ", " +
          ping_response.serial_number)
    # print(ping_response.to_string()) # alternative to above


def callback(result):
    if result == ximu3.RESULT_OK:
        print_ping_response(connection.ping())
    else:
        print("Unable to open connection")


if helpers.ask_question("Use async implementation?"):
    connection.open_async(callback)
    time.sleep(3)
else:
    if connection.open() == ximu3.RESULT_OK:
        print_ping_response(connection.ping())
    else:
        print("Unable to open connection")

# Close connection
connection.close()
