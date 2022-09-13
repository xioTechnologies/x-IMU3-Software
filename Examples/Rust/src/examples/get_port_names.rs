use ximu3::port_scanner::*;

pub fn run() {
    let port_names = PortScanner::get_port_names();

    if port_names.is_empty() {
        println!("No ports available")
    }

    for port_name in port_names {
        println!("{}", port_name);
    }
}
