import connection
import helpers
import ximu3

if helpers.ask_question("Search for connections?"):
    print("Searching for connections")

    devices = ximu3.PortScanner.scan_filter(ximu3.CONNECTION_TYPE_BLUETOOTH)

    if not devices:
        raise Exception("No Bluetooth connections available")

    print("Found " + devices[0].device_name + " " + devices[0].serial_number)

    connection.run(devices[0].connection_info)
else:
    connection.run(ximu3.BluetoothConnectionInfo("COM1"))
