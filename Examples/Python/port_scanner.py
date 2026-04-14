import time

import ximu3


# Blocking
def print_devices(devices: list[ximu3.Device]) -> None:
    print(f"{len(devices)} device(s) found")

    for device in devices:
        print(", ".join([device.device_name, device.serial_number, str(device.connection_config)]))
        # print(device)  # alternative to above


devices = ximu3.PortScanner.scan()

print_devices(devices)


# Non-blocking
def callback(devices: list[ximu3.Device]) -> None:
    print_devices(devices)


_ = ximu3.PortScanner(callback)

time.sleep(60)
