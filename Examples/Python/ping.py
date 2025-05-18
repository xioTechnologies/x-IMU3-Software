import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.CONNECTION_TYPE_USB)

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0].device_name} {devices[0].serial_number}")

# Open connection
connection = ximu3.Connection(devices[0].connection_info)

if connection.open() != ximu3.RESULT_OK:
    raise Exception("Unable to open connection")

response = connection.ping()

if response.result == ximu3.RESULT_OK:
    print(", ".join([response.interface, response.device_name, response.serial_number]))
else:
    print("No response")

# Close connection
connection.close()
