import ximu3

port_names = ximu3.SerialDiscovery.get_available_ports()

if not port_names:
    print("No ports available")
else:
    for port_name in port_names:
        print(port_name)
