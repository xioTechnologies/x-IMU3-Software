import helpers
import ximu3


def callback(result):
    print(result)


# Open all USB connections
devices = ximu3.PortScanner.scan_filter("USB")
connections = []

for device in devices:
    print(device.to_string())

    connection = ximu3.Connection(device.connection_info)

    if connection.open() == "Ok":
        connections.append(connection)
    else:
        raise Exception("Unable to open connection")

# Log data
directory = "C:/"
name = "Data Logger Example"

if helpers.yes_or_no("Use async implementation?"):
    data_logger = ximu3.DataLogger(directory, name, connections, callback)
    helpers.wait(3)
    del data_logger
else:
    print(ximu3.DataLogger.log(directory, name, connections, 3))

# Close all connections
for connection in connections:
    connection.close()
