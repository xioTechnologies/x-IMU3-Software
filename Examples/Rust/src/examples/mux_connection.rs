use super::connection;
use ximu3::connection::*;
use ximu3::connection_config::*;
use ximu3::port_scanner::*;

pub fn run() {
    // Search for connection
    let devices = PortScanner::scan_filter(PortType::Usb);

    let Some(device) = devices.first() else {
        println!("No USB connections available");
        return;
    };

    println!("Found {device}");

    // Open connection
    let usb_connection = Connection::new(&device.connection_config);

    if let Err(error) = usb_connection.open() {
        println!("Unable to open {}. {error}.", usb_connection.get_config());
        return;
    }

    // Mux connection
    let config = &ConnectionConfig::MuxConnectionConfig(MuxConnectionConfig::new(0x41, &usb_connection)); // replace with actual connection config

    connection::run(config);

    // Close connection
    usb_connection.close();
}
