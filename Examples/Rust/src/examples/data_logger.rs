use crate::helpers;
use ximu3::connection::*;
use ximu3::connection_type::*;
use ximu3::data_logger::*;
use ximu3::port_scanner::*;

pub fn run() {
    // Open all USB connections
    let mut connections = vec![];

    for device in PortScanner::scan_filter(ConnectionType::Usb) {
        println!("{}", device);

        let connection = Connection::new(&device.connection_info);

        if connection.open().is_ok() {
            connections.push(connection);
        } else {
            println!("Unable to open connection");
        }
    }

    if connections.is_empty() {
        println!("No USB connections available");
        return;
    }

    // Log data
    let destination = "C:/";
    let name = "Data Logger Example";

    if helpers::yes_or_no("Use async implementation?") {
        let data_logger = DataLogger::new(destination, name, connections.iter().collect());

        if data_logger.is_ok() {
            std::thread::sleep(std::time::Duration::from_secs(3));
        }

        print_result(&data_logger);

        drop(data_logger);
    } else {
        print_result(&DataLogger::log(destination, name, connections.iter().collect(), 3));
    }

    // Close all connections
    for connection in connections.iter() {
        connection.close();
    }
}

fn print_result<T>(result: &Result<T, ()>) {
    match result {
        Ok(_) => println!("OK"),
        Err(_) => println!("Error"),
    }
}
