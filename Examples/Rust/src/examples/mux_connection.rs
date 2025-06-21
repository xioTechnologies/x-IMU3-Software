use super::connection;
use ximu3::connection::*;
use ximu3::connection_info::*;
use ximu3::connection_type::*;
use ximu3::port_scanner::*;

pub fn run() {
    // Search for connection
    let devices = PortScanner::scan_filter(ConnectionType::Usb);

    if devices.is_empty() {
        println!("No USB connections available");
        return;
    }

    let device = devices.first().unwrap();

    println!("Found {} {}", device.device_name, device.serial_number);

    // Open connection
    let usb_connection = Connection::new(&device.connection_info);

    if usb_connection.open().is_err() {
        println!("Unable to open connection");
        return;
    }

    // Mux connection
    let mux_connection_info = &ConnectionInfo::MuxConnectionInfo(MuxConnectionInfo::new(0x00, &usb_connection)); // replace with actual connection info

    connection::run(mux_connection_info);

    mux_connection.close();

    // Close connection
    usb_connection.close();
}
