import connection
import helpers
import ximu3

if helpers.yes_or_no("Search for connections?"):
    devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_SERIAL)

    if not devices:
        raise Exception("No serial connections available")

    print(f"Found {devices[0]}")

    config = devices[0].connection_config

    connection.run(config)
else:
    config = ximu3.SerialConnectionConfig("COM1", 115200, False)  # replace with actual connection config

    connection.run(config)
