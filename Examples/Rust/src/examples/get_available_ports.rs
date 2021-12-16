use ximu3::serial_discovery::*;

pub fn run() {
    let port_names = SerialDiscovery::get_available_ports();

    if port_names.is_empty() {
        println!("No ports available")
    }

    for port_name in port_names {
        println!("{}", port_name);
    }
}
