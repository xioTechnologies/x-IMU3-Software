import time

import helpers
import ximu3

# Open all USB connections
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)
connections = []

for device in devices:
    print(f"Found {device}")

    connection = ximu3.Connection(device.connection_info)

    result = connection.open()

    if result == ximu3.RESULT_OK:
        connections.append(connection)
    else:
        raise Exception(f"Unable to open {connection.get_info().to_string()}. {ximu3.result_to_string(result)}.")

if not connections:
    raise Exception("No USB connections available")

# Log data
destination = "C:/"
name = "x-IMU3 Data Logger Example"

if helpers.yes_or_no("Use async implementation?"):
    data_logger = ximu3.DataLogger(destination, name, connections)

    result = data_logger.get_result()

    if result != ximu3.RESULT_OK:
        raise Exception(f"Data logger failed. {ximu3.result_to_string(result)}.")

    time.sleep(3)

    del data_logger
else:
    result = ximu3.DataLogger.log(destination, name, connections, 3)

    if result != ximu3.RESULT_OK:
        raise Exception(f"Data logger failed. {ximu3.result_to_string(result)}.")

# Close all connections
for connection in connections:
    connection.close()
