import time

import helpers
import ximu3

# Open all USB connections
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)
connections = []

for device in devices:
    print(f"Found {device}")

    connection = ximu3.Connection(device.connection_config)

    connection.open()

    connections.append(connection)

if not connections:
    raise Exception("No USB connections available")

# Log data
destination = "C:/"
name = "x-IMU3 Data Logger Example"

if helpers.yes_or_no("Use async implementation?"):
    data_logger = ximu3.DataLogger(destination, name, connections)

    time.sleep(3)

    del data_logger
else:
    ximu3.DataLogger.log(destination, name, connections, 3)

# Close all connections
for connection in connections:
    connection.close()
