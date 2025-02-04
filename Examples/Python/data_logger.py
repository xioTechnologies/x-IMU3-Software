import helpers
import time
import ximu3

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
destination = "C:/"
name = "Data Logger Example"

if helpers.yes_or_no("Use async implementation?"):
    data_logger = ximu3.DataLogger(destination, name, connections)

    result = data_logger.get_result()

    if result == ximu3.RESULT_OK:
        time.sleep(3)

    print(ximu3.result_to_string(result))

    del data_logger
else:
    print(ximu3.result_to_string(ximu3.DataLogger.log(destination, name, connections, 3)))

# Close all connections
for connection in connections:
    connection.close()
