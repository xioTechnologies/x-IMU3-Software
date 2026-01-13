use super::connection;
use ximu3::connection::*;
use ximu3::connection_info::*;
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
    let usb_connection = Connection::new(&device.connection_info);

    if let Err(error) = usb_connection.open() {
        println!("Unable to open {}. {error}.", usb_connection.get_info());
        return;
    }

    // Mux connection
    let connection_info = &ConnectionInfo::MuxConnectionInfo(MuxConnectionInfo::new(0x41, &usb_connection)); // replace with actual connection info

    connection::run(connection_info);

    // Close connection
    usb_connection.close();
}
