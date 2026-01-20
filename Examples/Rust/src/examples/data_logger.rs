use crate::helpers;
use ximu3::connection::*;
use ximu3::data_logger::*;
use ximu3::port_scanner::*;

pub fn run() {
    // Open all USB connections
    let mut connections = vec![];

    for device in PortScanner::scan_filter(PortType::Usb) {
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

    // Log data
    let destination = "C:/";
    let name = "x-IMU3 Data Logger Example";

    if helpers::yes_or_no("Use async implementation?") {
        let data_logger = DataLogger::new(destination, name, connections.iter().collect());

        if let Err(error) = data_logger {
            println!("Data logger failed. {error}.");
            return;
        }

        std::thread::sleep(std::time::Duration::from_secs(3));

        drop(data_logger);
    } else {
        let result = DataLogger::log(destination, name, connections.iter().collect(), 3);

        if let Err(error) = result {
            println!("Data logger failed. {error}.");
            return;
        }
    }

    println!("Complete");

    // Close all connections
    for connection in connections.iter() {
        connection.close();
    }
}
