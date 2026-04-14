use ximu3::connection::*;
use ximu3::data_logger::*;
use ximu3::port_scanner::*;

pub fn run() {
    // Open all USB connections
    let mut connections = vec![];

    let devices = PortScanner::scan_filter(PortType::Usb);

    std::thread::sleep(std::time::Duration::from_secs(1)); // wait for OS to release port

    for device in devices {
        println!("Found {device}");

        let connection = Connection::new(&device.connection_config);

        if let Err(error) = connection.open() {
            println!("Unable to open {}. {error}.", connection.get_config());
        } else {
            connections.push(connection);
        }
    }

    if connections.is_empty() {
        println!("No USB connections available");
        return;
    }

    // Log data (blocking)
    let destination = "C:/";
    let name_blocking = "x-IMU3 Data Logger Example Blocking";

    let result = DataLogger::log(destination, name_blocking, connections.iter().collect(), 3);

    if let Err(error) = result {
        println!("Data logger failed. {error}.");
    }

    // Log data (non-blocking)
    let name_non_blocking = "x-IMU3 Data Logger Example Non-Blocking";

    let data_logger = DataLogger::new(destination, name_non_blocking, connections.iter().collect());

    if let Err(ref error) = data_logger {
        println!("Data logger failed. {error}.");
    }

    std::thread::sleep(std::time::Duration::from_secs(3));

    if let Ok(data_logger) = data_logger {
        drop(data_logger); // stop logging
    }

    println!("Complete");

    // Close all connections
    for connection in connections.iter() {
        connection.close();
    }
}
