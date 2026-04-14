import time

import ximu3

# Open all USB connections
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

time.sleep(1)  # wait for OS to release port

connections: list[ximu3.Connection] = []

for device in devices:
    print(f"Found {device}")

    connection = ximu3.Connection(device.connection_config).open()

    connections.append(connection)

if not connections:
    raise Exception("No USB connections available")

# Log data (blocking)
destination = "C:/"
name_blocking = "x-IMU3 Data Logger Example Blocking"

ximu3.DataLogger.log(destination, name_blocking, connections, 3)

# Log data (non-blocking)
name_non_blocking = "x-IMU3 Data Logger Example Non-Blocking"

data_logger = ximu3.DataLogger(destination, name_non_blocking, connections)

time.sleep(3)

del data_logger  # stop logging

print("Complete")

# Close all connections
for connection in connections:
    connection.close()
