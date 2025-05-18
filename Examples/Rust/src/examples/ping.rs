use ximu3::connection::*;
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
    let connection = Connection::new(&device.connection_info);

    if connection.open().is_err() {
        println!("Unable to open connection");
        return;
    }

    // Ping
    if let Ok(response) = connection.ping() {
        println!("{}, {}, {}", response.interface, response.device_name, response.serial_number);
        // println!("{}", response); // alternative to above
    } else {
        println!("No response");
    }

    // Close connection
    connection.close();
}
