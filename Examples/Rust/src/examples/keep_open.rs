use ximu3::connection::*;
use ximu3::connection_status::*;
use ximu3::keep_open::*;
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
    let connection = Connection::new(&device.connection_config);

    connection.add_status_closure(Box::new(|status| match status {
        ConnectionStatus::Connected => println!("Connected"),
        ConnectionStatus::Disconnected => println!("Reconnecting"),
    }));

    let keep_open = KeepOpen::new(&connection);

    // Close connection
    std::thread::sleep(std::time::Duration::from_secs(60));

    drop(keep_open);
}
