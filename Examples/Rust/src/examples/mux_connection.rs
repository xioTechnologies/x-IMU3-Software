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

    println!("Found {} {}", device.device_name, device.serial_number);

    // Open connection
    let usb_connection = Connection::new(&device.connection_info);

    if usb_connection.open().is_err() {
        println!("Unable to open connection");
        return;
    }

    // Mux connection
    let connection_info = &ConnectionInfo::MuxConnectionInfo(MuxConnectionInfo::new(0x41, &usb_connection)); // replace with actual connection info

    connection::run(connection_info);

    // Close connection
    usb_connection.close();
}
