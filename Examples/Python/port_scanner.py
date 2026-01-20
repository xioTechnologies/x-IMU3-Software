import time

import helpers
import ximu3


def print_devices(devices: list[ximu3.Device]) -> None:
    for device in devices:
        print(", ".join([device.device_name, device.serial_number, device.connection_config]))
        # print(device)  # alternative to above


def callback(devices: list[ximu3.Device]) -> None:
    print(f"Devices updated ({len(devices)} devices available)")

    print_devices(devices)


if helpers.yes_or_no("Use async implementation?"):
    _ = ximu3.PortScanner(callback)

    time.sleep(60)
else:
    devices = ximu3.PortScanner.scan()

    print("Found " + str(len(devices)) + " devices")

    print_devices(devices)
