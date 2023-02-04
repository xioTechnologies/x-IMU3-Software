import helpers
import time
import ximu3


def print_devices(devices):
    for device in devices:
        print(device.device_name + ", " +
              device.serial_number + ", " +
              device.connection_info.to_string())
        # print(device.to_string())  # alternative to above


def callback(devices):
    print("Devices updated (" + str(len(devices)) + " devices available)")
    print_devices(devices)


if helpers.yes_or_no("Use async implementation?"):
    _ = ximu3.PortScanner(callback)
    time.sleep(60)
else:
    devices = ximu3.PortScanner.scan()
    print("Found " + str(len(devices)) + " devices")
    print_devices(devices)
