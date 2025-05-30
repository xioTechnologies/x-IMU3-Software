import time

import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.CONNECTION_TYPE_USB)

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0].device_name} {devices[0].serial_number}")

# Open connection
connection = ximu3.Connection(devices[0].connection_info)


def callback(result):
    match result:
        case ximu3.RESULT_OK:
            print("OK")
        case ximu3.RESULT_ERROR:
            print("Error")

    # print(ximu3.result_to_string(result))  # alternative to above


connection.open_async(callback)

# Close connection
time.sleep(3)
connection.close()
