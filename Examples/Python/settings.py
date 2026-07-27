import time

import ximu3

# Search for connection
devices = ximu3.PortScanner.scan_filter(ximu3.PORT_TYPE_USB)

time.sleep(1)  # wait for OS to release port

if not devices:
    raise Exception("No USB connections available")

print(f"Found {devices[0]}")

# Open connection
connection = ximu3.Connection(devices[0].connection_config).open()

# Backup settings
file_path = "C:/Users/Public/x-IMU3 Example Settings.json"

ximu3.settings_backup(file_path, connection)

# Restore settings
ximu3.settings_restore(file_path, connection)

# Save command
connection.send_command('{"save":null}')

# Close connection
connection.close()
