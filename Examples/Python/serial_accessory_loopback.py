import time

import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0].device_name} {devices[0].serial_number}")

# Open connection
connection = ximu3.Connection(devices[0].connection_info)

if connection.open() != ximu3.RESULT_OK:
    raise Exception("Unable to open connection")


# Loopback
def serial_accessory_callback(message: ximu3.SerialAccessoryMessage):
    pass


connection.add_serial_accessory_callback(serial_accessory_callback)

# tx_data = [0x00 .. 0xFF]
# rx_data = []

# def callback(data):
# 	rx_data += data

# for (int index = 0; index < 256; index +=32)
# 	tx_data = byte
# 	send(bytes_to_json_string(tx_data))

# wait(1)

# print(tx_data)
# print(rx_data)

# if tx_data != rx_data:
# 	print("uh oh")


# Close connection
connection.close()
