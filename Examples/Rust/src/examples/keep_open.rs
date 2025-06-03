use ximu3::connection::*;
use ximu3::connection_type::*;
use ximu3::keep_open::*;
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

    let closure = Box::new(|status| {
        match status {
            ConnectionStatus::Connected => println!("Connected"),
            ConnectionStatus::Reconnecting => println!("Reconnecting"),
        }
        // println!("{}", status); // alternative to above
    });

    let keep_open = KeepOpen::new(&connection, closure);

    // Close connection
    std::thread::sleep(std::time::Duration::from_secs(60));
    drop(keep_open);
}
