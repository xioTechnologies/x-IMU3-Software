use ximu3::connection::*;
use ximu3::connection_type::*;
use ximu3::data_logger::*;
use ximu3::port_scanner::*;
use crate::helpers;

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

    // Log data
    let directory = "C:/";
    let name = "Data Logger Example";

    if helpers::ask_question("Use async implementation?") {
        let data_logger = DataLogger::new(directory, name, connections.iter().collect());

        if data_logger.is_ok() {
            std::thread::sleep(std::time::Duration::from_secs(3));
        }

        print_result(&data_logger);

        drop(data_logger);
    } else {
        print_result(&DataLogger::log(directory, name, connections.iter().collect(), 3));
    }

    // Close all connections
    for connection in connections.iter() {
        connection.close();
    }
}

fn print_result<T>(result: &Result<T, ()>) {
    match result {
        Ok(_) => println!("Ok"),
        Err(_) => println!("Error"),
    }
}
