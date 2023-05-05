import helpers
import time
import ximu3


def callback(result):
    print(ximu3.result_to_string(result))


# Open all USB connections
devices = ximu3.PortScanner.scan_filter(ximu3.CONNECTION_TYPE_USB)
connections = []

for device in devices:
    print(device.to_string())

    connection = ximu3.Connection(device.connection_info)

    if connection.open() == ximu3.RESULT_OK:
        connections.append(connection)
    else:
        raise Exception("Unable to open connection")

# Log data
directory = "C:/"
name = "Data Logger Example"

if helpers.ask_question("Use async implementation?"):
    data_logger = ximu3.DataLogger(directory, name, connections, callback)
    time.sleep(3)
    del data_logger
else:
    print(ximu3.result_to_string(ximu3.DataLogger.log(directory, name, connections, 3)))

# Close all connections
for connection in connections:
    connection.close()
