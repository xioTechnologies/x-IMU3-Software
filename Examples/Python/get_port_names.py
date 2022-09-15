import ximu3

port_names = ximu3.PortScanner.get_port_names()

if not port_names:
    print("No ports available")
else:
    for port_name in port_names:
        print(port_name)
