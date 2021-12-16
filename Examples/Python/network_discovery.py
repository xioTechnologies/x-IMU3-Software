import helpers
import ximu3


def print_devices(devices):
    for device in devices:
        print(device.device_name + " - " +
              device.serial_number + ", RSSI: " +
              str(device.rssi) + "%, Battery: " +
              str(device.battery) + "%, " +
              device.status + ", " +
              device.tcp_connection_info.to_string() + ", " +
              device.udp_connection_info.to_string())
        # print(device.to_string())  # alternative to above


def callback(devices):
    print("Devices updated (" + str(len(devices)) + " devices available)")
    print_devices(devices)


if helpers.yes_or_no("Use async implementation?"):
    _ = ximu3.NetworkDiscovery(callback)
    helpers.wait(10)
else:
    devices = ximu3.NetworkDiscovery.scan(2000)
    print("Discovered " + str(len(devices)) + " devices")
    print_devices(devices)
