use ximu3::connection::*;
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
    let connection = Connection::new(&device.connection_info);

    let connection_info = connection.get_info();

    let closure = Box::new(move |result| {
        if let Err(error) = result {
            println!("Unable to open {connection_info}. {error}.");
            return;
        }

        println!("Ok")
    });

    connection.open_async(closure);

    // Close connection
    std::thread::sleep(std::time::Duration::from_secs(3));

    connection.close();
}
