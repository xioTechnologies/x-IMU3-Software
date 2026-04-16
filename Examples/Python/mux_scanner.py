import time

import ximu3

# Search for connection
usb_devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

time.sleep(1)  # wait for OS to release port

if not usb_devices:
    raise Exception("No USB connections available")

print(f"Found {usb_devices[0]}")

# Open connection
usb_connection = ximu3.Connection(usb_devices[0].connection_config).open()


# Mux scanner (blocking)
def print_devices(devices: list[ximu3.Device]) -> None:
    print(f"{len(devices)} device(s) found")

    for device in devices:
        print(", ".join([device.device_name, device.serial_number, str(device.connection_config)]))
        # print(device)  # alternative to above


mux_devices = ximu3.MuxScanner.scan(usb_connection)

print_devices(mux_devices)


# Mux scanner (non-blocking)
def callback(mux_devices: list[ximu3.Device]) -> None:
    print_devices(mux_devices)


_ = ximu3.MuxScanner(usb_connection, callback)

time.sleep(60)

# Close connection
usb_connection.close()
