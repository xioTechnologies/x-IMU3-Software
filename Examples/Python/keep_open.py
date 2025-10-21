import time

import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0].device_name} {devices[0].serial_number}")

# Open connection
connection = ximu3.Connection(devices[0].connection_info)


def callback(status: int) -> None:
    match status:
        case ximu3.CONNECTION_STATUS_CONNECTED:
            print("Connected")
        case ximu3.CONNECTION_STATUS_RECONNECTING:
            print("Reconnecting")

    # print(ximu3.connection_status_to_string(status))  # alternative to above


_ = ximu3.KeepOpen(connection, callback)

# Close connection
time.sleep(60)
connection.close()
